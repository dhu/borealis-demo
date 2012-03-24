#ifndef  VIDEOMARSHAL_H
#define  VIDEOMARSHAL_H

#include  "MedusaClient.h"
#include  "TupleHeader.h"

////////////////////////////////////////////////////////////////////////////////
//
// Generated Martialing code for the Video program.
//..............................................................................


using namespace Borealis;

class VideoMarshal
{
  public:

    VideoMarshal() {};
   ~VideoMarshal() {};

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

    struct ccd_schema
    {
        int32    contrast;
        uint8    *ccd_frame;
    } __attribute__((__packed__));

    struct  Ccdstream : public TupleHeader
    {
        ccd_schema   _data;
    } __attribute__((__packed__));

    /// The sentCcdstream  method must be defined by the application.
    /// It is called after sendCcdstream is done and a pause.
    ///
    void  sentCcdstream();

    /// Enque a Ccdstream for input.
    ///
    void  batchCcdstream(Ccdstream  *tuple);

    /// Send enqued Ccdstream inputs.
    ///
    void  sendCcdstream(uint32  sleep);

  private:

    /// Connect the Ccdstream input stream.
    ///
    void  connectCcdstream();

    ///  Resume here.  Extend with a user callback.
    void  delayCcdstream();

  public:

    struct joined_schema
    {
        int32    contrast;
        uint8    *ccd_frame;
        int32    timestamp;
        uint8    *ir_frame;
    } __attribute__((__packed__));

    struct  Contraststream : public TupleHeader
    {
        joined_schema   _data;
    } __attribute__((__packed__));

    /// The sentContraststream  method must be defined by the application.
    /// It is called after sendContraststream is done and a pause.
    ///
    void  sentContraststream();

    /// Enque a Contraststream for input.
    ///
    void  batchContraststream(Contraststream  *tuple);

    /// Send enqued Contraststream inputs.
    ///
    void  sendContraststream(uint32  sleep);

  private:

    /// Connect the Contraststream input stream.
    ///
    void  connectContraststream();

    ///  Resume here.  Extend with a user callback.
    void  delayContraststream();

  public:

    struct ir_schema
    {
        int32    timestamp;
        uint8    *ir_frame;
    } __attribute__((__packed__));

    struct  Irstream : public TupleHeader
    {
        ir_schema   _data;
    } __attribute__((__packed__));

    /// The sentIrstream  method must be defined by the application.
    /// It is called after sendIrstream is done and a pause.
    ///
    void  sentIrstream();

    /// Enque a Irstream for input.
    ///
    void  batchIrstream(Irstream  *tuple);

    /// Send enqued Irstream inputs.
    ///
    void  sendIrstream(uint32  sleep);

  private:

    /// Connect the Irstream input stream.
    ///
    void  connectIrstream();

    ///  Resume here.  Extend with a user callback.
    void  delayIrstream();

  public:

    struct  Joinedstream : public TupleHeader
    {
        joined_schema   _data;
    } __attribute__((__packed__));

    /// The sentJoinedstream  method must be defined by the application.
    /// It is called after sendJoinedstream is done and a pause.
    ///
    void  sentJoinedstream();

    /// Enque a Joinedstream for input.
    ///
    void  batchJoinedstream(Joinedstream  *tuple);

    /// Send enqued Joinedstream inputs.
    ///
    void  sendJoinedstream(uint32  sleep);

  private:

    /// Connect the Joinedstream input stream.
    ///
    void  connectJoinedstream();

    ///  Resume here.  Extend with a user callback.
    void  delayJoinedstream();

  private:

    /// Handler to dispatch tuples received.
    ///
    static Status outputHandler(ptr<StreamEvent> event);


  public:

    struct selected_schema
    {
        int32    contrast;
        uint8    *selected_frame;
    } __attribute__((__packed__));

    /// The receivedSelectedstream method must be defined by the application.
    /// It is called after a(n) selected_schema is received.
    ///
    static void  receivedSelectedstream(selected_schema  *tuple);

  private:

    /// Subscribe to the Selectedstream output stream.
    ///
    void  subscribeSelectedstream();

    /// Handler to receive tuples from the Selectedstream stream.
    ///
    static Status  SelectedstreamHandler(ptr<StreamEvent>  event);

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
    ptr<StreamEvent>   _eventCcdstream;
    ptr<StreamEvent>   _eventContraststream;
    ptr<StreamEvent>   _eventIrstream;
    ptr<StreamEvent>   _eventJoinedstream;
};

#endif                         // VIDEOMARSHAL_H
