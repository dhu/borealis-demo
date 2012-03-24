#ifndef  HA_DEMOMARSHAL_H
#define  HA_DEMOMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Ha_Demo program.
//..............................................................................


using namespace Borealis;

class Ha_DemoMarshal
{
  public:

    Ha_DemoMarshal() {};
   ~Ha_DemoMarshal() {};

    /// Activate the front-end and subscribe to streams.
    /// Returns 0 if okay; else an error occured.
    ///
    int32  open();


    /// Connect to inputs and subscribe to streams only.
    /// Similar to open() but does not load the query.
    ///
    void  openInputOutput();


    /// Run the client event loop.
    /// This does not return unless terminated or on an exception.
    ///
    void  runClient();


    /// Terminate the client event loop.
    ///
    void  terminateClient();


    /// Copy a string value to a fixed length array and zero fill.
    ///
    static void  setStringField(string  value,
                                  char  field[],
                                uint32  length)
                          throw(AuroraException);


    /// Get the timestamp for a tuple.
    ///
    static timeval   getTimeValue(uint8  *tuple)
    {
       return(*((timeval *)(tuple - HEADER_SIZE)));
    }


  public:

    struct PacketSchema
    {
        int32    timestamp;
        int32    source;
    } __attribute__((__packed__));

    struct  Input1 : public TupleHeader
    {
        PacketSchema   _data;
    } __attribute__((__packed__));

    /// The sentInput1  method must be defined by the application.
    /// It is called after sendInput1 is done and a pause.
    ///
    void  sentInput1();

    /// Enque a Input1 for input.
    ///
    void  batchInput1(Input1  *tuple);

    /// Send enqued Input1 inputs.
    ///
    void  sendInput1(uint32  sleep);

  private:

    /// Connect the Input1 input stream.
    ///
    void  connectInput1();

    ///  Resume here.  Extend with a user callback.
    void  delayInput1();

  public:

    struct  Input2 : public TupleHeader
    {
        PacketSchema   _data;
    } __attribute__((__packed__));

    /// The sentInput2  method must be defined by the application.
    /// It is called after sendInput2 is done and a pause.
    ///
    void  sentInput2();

    /// Enque a Input2 for input.
    ///
    void  batchInput2(Input2  *tuple);

    /// Send enqued Input2 inputs.
    ///
    void  sendInput2(uint32  sleep);

  private:

    /// Connect the Input2 input stream.
    ///
    void  connectInput2();

    ///  Resume here.  Extend with a user callback.
    void  delayInput2();

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler(ptr<StreamEvent> event);


  public:

    struct CountSchema
    {
        int32    source;
        int32    timestamp;
        int32    agg_id;
        int32    count;
    } __attribute__((__packed__));

    /// The receivedOutput method must be defined by the application.
    /// It is called after a(n) CountSchema is received.
    ///
    static void  receivedOutput(CountSchema  *tuple);

  private:

    /// Subscribe to the Output output stream.
    ///
    void  subscribeOutput();

    /// Handler to receive tuples from the Output stream.
    ///
    static Status  OutputHandler(ptr<StreamEvent>  event);

  private:

    /// Launch the Borealis front-end.
    ///
    static int32  launchDiagram(string  xml);  // Path of an initial xml file.

    /// Client connections to Borealis nodes.
    ///
    MedusaClient      *_client;


    /// Event state for input streams.
    /// These are declared with a smart pointer as fast_post_event requires it.
    ///
    ptr<StreamEvent>   _eventInput1;
    ptr<StreamEvent>   _eventInput2;
};

#endif                         // HA_DEMOMARSHAL_H
