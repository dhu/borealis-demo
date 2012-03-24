#include  "Ha_DemoMarshal.h"
#include  "util.h"

#define HA_DEMO_XML     "ha_demo.xml ha_demo_deployment.xml"
#define MAX_BUFFER      640000000

#define HA_DEMO_ENDPOINT     "127.0.1.1:25000"

#define INPUT1  "input1"
#define INPUT2  "input2"
#define OUTPUT  "output"

////////////////////////////////////////////////////////////////////////////////
//
// Generated marshaling code for the Ha_Demo program.
//..............................................................................


////////////////////////////////////////////////////////////////////////////////
//
//  Subscribe to input and output streams.
//
int32  Ha_DemoMarshal::open()
{
    int32   status;
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeOutput();

    //  Launch the front-end with the xml file.
    //  Creates a client that communicates with a borealis node at ip
    //  though the given port.
    //  Then it sends the network diagram as XML strings
    //  and runs start_query.
    //  It waits until the front-end terminates and the port is now free.
    //
    status = launchDiagram(HA_DEMO_XML);

    if (status)
    {   ERROR << "launchDiagram failed ( " << status << " ).";
    }
    else
    {   // Establish input data path connections.
        connectInput1();
        connectInput2();
    }

    return  status;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Connect to inputs, subscribe to outputs.
//
void  Ha_DemoMarshal::openInputOutput()
{
//..............................................................................


    // Open a client to send and receive data.
    //
    _client = new MedusaClient(InetAddress());

    // Subscribe to outputs.
    //
    subscribeOutput();

    {   // Establish input data path connections.
        connectInput1();
        connectInput2();
    }
    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Activate the client I/O event loop.
void  Ha_DemoMarshal::runClient()
{
//..............................................................................


    //  This does not return unless terminated or on an exception.
    _client->run();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Terminate the client I/O event loop.
void  Ha_DemoMarshal::terminateClient()
{
//..............................................................................


    _client->terminate();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Copy a string value to a fixed length array and zero fill.
//
void  Ha_DemoMarshal::setStringField(string  value,
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
void  Ha_DemoMarshal::connectInput1()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 20000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventInput1 = ptr<StreamEvent>(new StreamEvent(INPUT1));
        _eventInput1->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Ha_DemoMarshal::batchInput1( Input1  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventInput1->insert_bin(string((const char *)tuple,
                                  sizeof(Input1)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Ha_DemoMarshal::sendInput1( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventInput1);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventInput1);
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
                             wrap(this, &Ha_DemoMarshal::delayInput1)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  Ha_DemoMarshal::delayInput1()
{
//..............................................................................


    // Release the previous event.
    //
    _eventInput1.reset();

    // Construct a new Input1 input event.
    //
    _eventInput1 = ptr<StreamEvent>(new StreamEvent(INPUT1));
    _eventInput1->_inject = true;

    // Return to the application code.
    //
    sentInput1();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Ha_DemoMarshal::connectInput2()
{
//..............................................................................


    // Starting to produce events on input stream.
    //
    if (!_client->set_data_path(MAX_BUFFER, Util::get_host_address("127.0.1.1"), 20000))
    {   ERROR << "Failed setting data path";
    }
    else
    {   DEBUG << "Set data path";

        _eventInput2 = ptr<StreamEvent>(new StreamEvent(INPUT2));
        _eventInput2->_inject = true;
    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Ha_DemoMarshal::batchInput2( Input2  *tuple )
{
//..............................................................................


    // Tuples are buffered in a string.
    //
    _eventInput2->insert_bin(string((const char *)tuple,
                                  sizeof(Input2)));

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
void  Ha_DemoMarshal::sendInput2( uint32  sleep )
{
//..............................................................................


    // Transmit data to the node.
    Status  status = _client->fast_post_event(_eventInput2);

    while (!status)
    {   if (status.as_string() == DataHandler::NO_SPACE)
        {   // Wait if no more space in buffer.
            // At this point the data was never put in the buffer.
            // It needs to be requeued unless we want to drop it.
            WARN << "We dropped a tuple.";
            Thread::sleep(Time::msecs(sleep));

            // retry (make this conditional).
            status = _client->fast_post_event(_eventInput2);
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
                             wrap(this, &Ha_DemoMarshal::delayInput2)))
                  ->arm(Time::now() + Time::msecs(sleep));

    }

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Resume here after sending a tuple.
//
void  Ha_DemoMarshal::delayInput2()
{
//..............................................................................


    // Release the previous event.
    //
    _eventInput2.reset();

    // Construct a new Input2 input event.
    //
    _eventInput2 = ptr<StreamEvent>(new StreamEvent(INPUT2));
    _eventInput2->_inject = true;

    // Return to the application code.
    //
    sentInput2();

    return;
}



////////////////////////////////////////////////////////////////////////////////
//
// Dispatch output on our fast datapath to a handler.
//
Status  Ha_DemoMarshal::outputHandler(ptr<StreamEvent>  event)
{
//..............................................................................


    if (event->_stream == Name(OUTPUT))
    {   return Ha_DemoMarshal::OutputHandler(event);
    }

    return  string("Unknown output stream ") + to_string(event->_stream);
}



////////////////////////////////////////////////////////////////////////////////
//
// Subscribing to receive output on a fast datapath.
//
void  Ha_DemoMarshal::subscribeOutput()
{
//..............................................................................


    DEBUG << "Subscribing to receive output.";

    // Setup the subscription request.
    Status  status = _client->set_data_handler(
                                  InetAddress(Util::form_endpoint(HA_DEMO_ENDPOINT,
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
Status  Ha_DemoMarshal::OutputHandler(ptr<StreamEvent>  event)
{
    CountSchema  *tuple;
    int32         index;
    uint32        offset = 0;
//..............................................................................


    // For each tuple that was received,
    //
    for (index = 0; index < event->_inserted_count; index++)
    {
        offset += HEADER_SIZE;

        // At the tuple data past the header.
        //
        tuple = (CountSchema *)&event->_bin_tuples[offset];
        DEBUG << "DATA:  " << to_hex_string(tuple, sizeof(CountSchema));

        receivedOutput(tuple);
        offset += sizeof(CountSchema);
    }

    // Signal done with this batch.
    //
    return  true;
}



////////////////////////////////////////////////////////////////////////////////
//
// Launch the Borealis front-end.
//
int32  Ha_DemoMarshal::launchDiagram(string  xml)  // Path of an initial xml file.
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

////////////////////////////  end  Ha_DemoMarshal.cc  //////////////////////////
