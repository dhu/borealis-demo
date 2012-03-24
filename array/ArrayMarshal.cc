#include  "ArrayMarshal.h"
#include  "util.h"

#define ARRAY_XML     "Array.xml ArrayDeploy.xml"
#define MAX_BUFFER      640000000

#define ARRAY_ENDPOINT     "127.0.1.1:25000"

#define PACKET  "packet"
#define RESULT  "result"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Array program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  ArrayMarshal::open()
{
    int32   status;
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeResult();

    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    status = launchDiagram(ARRAY_XML);

    if (status)
    {   ERROR << "launchDiagram failed ( " << status << " ).";
    }
    else
    {   // Establish input data path connections.
        connectPacket();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  ArrayMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeResult();

    {   // Establish input data path connections.
        connectPacket();
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  ArrayMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  ArrayMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  ArrayMarshal::setStringField(string  value,
                                       char  field[],
                                     uint32  length)
                              throw(AuroraException)
{
//..............................................................................


    if (value.length() > length)
    {   Throw(AuroraException,
              "Protocol string over " + to_string(length) + ".");
    }

    strncpy(field, value.c_str(), length);

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  ArrayMarshal::connectPacket()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventPacket = ptr<StreamEvent>(new StreamEvent(PACKET));
        _eventPacket->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  ArrayMarshal::batchPacket( Packet  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 230400;
    _eventPacket->insert_array(string((const char *)tuple->_data.value, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.value = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventPacket->insert_bin(string((const char *)tuple,
                                  sizeof(Packet)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  ArrayMarshal::sendPacket( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventPacket);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventPacket);
        }
        else
        {   ERROR << "Connection closed... stopping event stream";
            return;
        }
    }

    if (sleep)
    {    // The event loop is activated so that the queue can be processed.
         // The callback is enqueued with a timer.
         // We only callback with a timer because this is looping.
         // We also need a delayed callback so the queue can be processed.
         // If we just go to sleep the event loop will not be run.
         //
         (new CallbackTimer(_client->get_loop(),
                             wrap(this, &ArrayMarshal::delayPacket)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  ArrayMarshal::delayPacket()
{
//..............................................................................


    // Release the previous event.
    //
    _eventPacket.reset();

    // Construct a new Packet input event.
    //
    _eventPacket = ptr<StreamEvent>(new StreamEvent(PACKET));
    _eventPacket->_inject = true;

    // Return to the application code.
    //
    sentPacket();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  ArrayMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    if (event->_stream == Name(RESULT))
    {   return ArrayMarshal::ResultHandler(event);
    }

    return  string("Unknown output stream ") + to_string(event->_stream);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  ArrayMarshal::subscribeResult()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(ARRAY_ENDPOINT,
                                                                  DEFAULT_MONITOR_PORT)),
                                  wrap(&outputHandler));

    if (status)
    {   DEBUG << "Done subscribing to output.";
    }
    else
    {   ERROR << "Could not subscribe to output.";
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Receive output on a fast datapath.
//
Status  ArrayMarshal::ResultHandler(ptr<StreamEvent>  event)
{
    PacketTuple  *tuple;
    int32         index;
    uint32        offset = 0;
    uint32        size   = 0;
    uint8        *array;
    uint32        size_value;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (PacketTuple *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(PacketTuple));

        // At the array data in the event.
        //
        array = (uint8 *)&(event->_bin_arrays[size]);

        // Offset past the array data.
        //
        size_value = *(uint32 *)&tuple->value;
        size += size_value;
        DEBUG << "size=" << size << "  size_value=" << size_value;

        // Modify the array field to reference the array data.
        //
        tuple->value = array;

        receivedResult(tuple);
        offset += sizeof(PacketTuple);
    }

    // Signal done with this batch.
    //
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  ArrayMarshal::launchDiagram(string  xml)  // Path of an initial xml file.
{
    int32   status;
    string  command;
//..............................................................................


    INFO << "Connect with: " << xml;

    command = string() + "BigGiantHead  " + xml;
    status  = system(command.c_str());

    DEBUG << "Ran the Borealis front end:  " << status;

    return  status;
}

////////////////////////////  end  ArrayMarshal.cc  //////////////////////////
