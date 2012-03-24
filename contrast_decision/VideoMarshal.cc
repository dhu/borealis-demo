#include  "VideoMarshal.h"
#include  "util.h"

#define VIDEO_XML     "Video.xml VideoDeploy.xml"
#define MAX_BUFFER      640000000

#define VIDEO_ENDPOINT     "127.0.1.1:25000"

#define CCDSTREAM  "ccdstream"
#define CONTRASTSTREAM  "contraststream"
#define IRSTREAM  "irstream"
#define JOINEDSTREAM  "joinedstream"
#define SELECTEDSTREAM  "selectedstream"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Video program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  VideoMarshal::open()
{
    int32   status;
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeSelectedstream();

    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    status = launchDiagram(VIDEO_XML);

    if (status)
    {   ERROR << "launchDiagram failed ( " << status << " ).";
    }
    else
    {   // Establish input data path connections.
        connectCcdstream();
        connectContraststream();
        connectIrstream();
        connectJoinedstream();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  VideoMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeSelectedstream();

    {   // Establish input data path connections.
        connectCcdstream();
        connectContraststream();
        connectIrstream();
        connectJoinedstream();
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  VideoMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  VideoMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  VideoMarshal::setStringField(string  value,
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
void  VideoMarshal::connectCcdstream()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventCcdstream = ptr<StreamEvent>(new StreamEvent(CCDSTREAM));
        _eventCcdstream->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::batchCcdstream( Ccdstream  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 304128;
    _eventCcdstream->insert_array(string((const char *)tuple->_data.ccd_frame, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.ccd_frame = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventCcdstream->insert_bin(string((const char *)tuple,
                                  sizeof(Ccdstream)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::sendCcdstream( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventCcdstream);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventCcdstream);
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
                             wrap(this, &VideoMarshal::delayCcdstream)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  VideoMarshal::delayCcdstream()
{
//..............................................................................


    // Release the previous event.
    //
    _eventCcdstream.reset();

    // Construct a new Ccdstream input event.
    //
    _eventCcdstream = ptr<StreamEvent>(new StreamEvent(CCDSTREAM));
    _eventCcdstream->_inject = true;

    // Return to the application code.
    //
    sentCcdstream();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::connectContraststream()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventContraststream = ptr<StreamEvent>(new StreamEvent(CONTRASTSTREAM));
        _eventContraststream->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::batchContraststream( Contraststream  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 304128;
    _eventContraststream->insert_array(string((const char *)tuple->_data.ccd_frame, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.ccd_frame = (uint8  *)size;

    // Inject the array data into the event.
    //
    size = 304128;
    _eventContraststream->insert_array(string((const char *)tuple->_data.ir_frame, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.ir_frame = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventContraststream->insert_bin(string((const char *)tuple,
                                  sizeof(Contraststream)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::sendContraststream( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventContraststream);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventContraststream);
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
                             wrap(this, &VideoMarshal::delayContraststream)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  VideoMarshal::delayContraststream()
{
//..............................................................................


    // Release the previous event.
    //
    _eventContraststream.reset();

    // Construct a new Contraststream input event.
    //
    _eventContraststream = ptr<StreamEvent>(new StreamEvent(CONTRASTSTREAM));
    _eventContraststream->_inject = true;

    // Return to the application code.
    //
    sentContraststream();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::connectIrstream()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventIrstream = ptr<StreamEvent>(new StreamEvent(IRSTREAM));
        _eventIrstream->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::batchIrstream( Irstream  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 304128;
    _eventIrstream->insert_array(string((const char *)tuple->_data.ir_frame, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.ir_frame = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventIrstream->insert_bin(string((const char *)tuple,
                                  sizeof(Irstream)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::sendIrstream( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventIrstream);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventIrstream);
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
                             wrap(this, &VideoMarshal::delayIrstream)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  VideoMarshal::delayIrstream()
{
//..............................................................................


    // Release the previous event.
    //
    _eventIrstream.reset();

    // Construct a new Irstream input event.
    //
    _eventIrstream = ptr<StreamEvent>(new StreamEvent(IRSTREAM));
    _eventIrstream->_inject = true;

    // Return to the application code.
    //
    sentIrstream();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::connectJoinedstream()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 15000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventJoinedstream = ptr<StreamEvent>(new StreamEvent(JOINEDSTREAM));
        _eventJoinedstream->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::batchJoinedstream( Joinedstream  *tuple )
{
    int32  size;
//..............................................................................


    // Inject the array data into the event.
    //
    size = 304128;
    _eventJoinedstream->insert_array(string((const char *)tuple->_data.ccd_frame, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.ccd_frame = (uint8  *)size;

    // Inject the array data into the event.
    //
    size = 304128;
    _eventJoinedstream->insert_array(string((const char *)tuple->_data.ir_frame, size));

    // Replace the array pointer with the array size in bytes.
    //
    tuple->_data.ir_frame = (uint8  *)size;

    // Tuples are buffered in a string.
    //
    _eventJoinedstream->insert_bin(string((const char *)tuple,
                                  sizeof(Joinedstream)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  VideoMarshal::sendJoinedstream( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventJoinedstream);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventJoinedstream);
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
                             wrap(this, &VideoMarshal::delayJoinedstream)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  VideoMarshal::delayJoinedstream()
{
//..............................................................................


    // Release the previous event.
    //
    _eventJoinedstream.reset();

    // Construct a new Joinedstream input event.
    //
    _eventJoinedstream = ptr<StreamEvent>(new StreamEvent(JOINEDSTREAM));
    _eventJoinedstream->_inject = true;

    // Return to the application code.
    //
    sentJoinedstream();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  VideoMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    if (event->_stream == Name(SELECTEDSTREAM))
    {   return VideoMarshal::SelectedstreamHandler(event);
    }

    return  string("Unknown output stream ") + to_string(event->_stream);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  VideoMarshal::subscribeSelectedstream()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(VIDEO_ENDPOINT,
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
Status  VideoMarshal::SelectedstreamHandler(ptr<StreamEvent>  event)
{
    selected_schema  *tuple;
    int32         index;
    uint32        offset = 0;
    uint32        size   = 0;
    uint8        *array;
    uint32        size_selected_frame;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (selected_schema *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(selected_schema));

        // At the array data in the event.
        //
        array = (uint8 *)&(event->_bin_arrays[size]);

        // Offset past the array data.
        //
        size_selected_frame = *(uint32 *)&tuple->selected_frame;
        size += size_selected_frame;
        DEBUG << "size=" << size << "  size_selected_frame=" << size_selected_frame;

        // Modify the array field to reference the array data.
        //
        tuple->selected_frame = array;

        receivedSelectedstream(tuple);
        offset += sizeof(selected_schema);
    }

    // Signal done with this batch.
    //
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  VideoMarshal::launchDiagram(string  xml)  // Path of an initial xml file.
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

////////////////////////////  end  VideoMarshal.cc  //////////////////////////
