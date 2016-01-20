#ifndef __simplecurl_h
#define __simplecurl_h

#include <wx/event.h>
#include <wx/datetime.h>
#include <wx/sstream.h>


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxSIMPLECURL_EVENT, 7578)
END_DECLARE_EVENT_TYPES()

class wxSimpleCurlEvent : public wxEvent
{
public:
	enum { STARTED, PROGRESS, FINISHED };

    wxSimpleCurlEvent(int id, wxEventType type, int code, const wxString &msg = wxEmptyString, const wxString &url = wxEmptyString )
        : wxEvent(id, type) { m_code = code; m_msg = msg; m_dt = wxDateTime::Now(); m_url = url; m_bytes = m_total = 0; }
	wxSimpleCurlEvent( const wxSimpleCurlEvent &evt )
		: wxEvent( evt.GetId(), evt.GetEventType() ),
		m_code(evt.m_code),
		m_msg(evt.m_msg),
		m_dt(evt.m_dt),
		m_url(evt.m_url),
		m_bytes(evt.m_bytes),
		m_total(evt.m_total)
	{ }

	int GetStatusCode() const { return m_code; }
    wxString GetMessage() const { return m_msg; }	
    virtual wxEvent* Clone() const { return new wxSimpleCurlEvent(*this); }
	wxString GetUrl() const { return m_url; }
	double GetBytesTransferred() const { return m_bytes; }
	double GetBytesTotal() const { return m_total; }

	void SetBytesTransferred( double b ) { m_bytes = b; }
	void SetBytesTotal( double t ) { m_total = t; }

protected:
	int m_code;
	wxString m_msg;
    wxDateTime m_dt;
	wxString m_url;
	double m_bytes, m_total;
};

typedef void (wxEvtHandler::*wxSimpleCurlEventFunction)(wxSimpleCurlEvent&);

#define wxSimpleCurlEventFunction(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxSimpleCurlEventFunction, &func)

#define EVT_SIMPLECURL(id, fn) \
    wx__DECLARE_EVT1(wxSIMPLECURL_EVENT, id, wxSimpleCurlEventFunction(fn))

class wxSimpleCurl : public wxObject
{
public:
	// app-wide init and shutdown calls for underlying libcurl initialization
	static void Init();
	static void Shutdown();
	static void SetProxyAddress( const wxString &proxy );
	static wxString GetProxyForURL( const wxString &url );
	static wxArrayString GetProxyAutodetectMessages();

	// geocoding function using google APIs.
	// call is synchronous.  Optionally determine time 
	// zone from lat/lon using second service call
	static bool GeoCode( const wxString &address, double *lat, double *lon, double *tz = 0);
	enum MapProvider { GOOGLE_MAPS, BING_MAPS };
	static wxBitmap StaticMap( double lat, double lon, int zoom, MapProvider service = BING_MAPS );



	wxSimpleCurl( wxEvtHandler *handler = 0, int id = wxID_ANY );
	virtual ~wxSimpleCurl();

	void SetPostData( const wxString &s ) { m_postData = s; }
	void AddHttpHeader( const wxString &s ) { m_httpHeaders.Add( s ); }

	// progress reporting methods
	// send wxSimpleCurlEvents to the specified wxEvtHandler, and events have the given id
	void SetEventHandler( wxEvtHandler *hh, int id );

	// receive notifications via a callback function
	void SetCallback( void (*function)( wxSimpleCurlEvent &, void * ), void *user_data );

	// returns true always for asynchronous calls
	bool Start( const wxString &url, bool synchronous=false );

	wxString GetDataAsString();
	wxImage GetDataAsImage( int bittype = wxBITMAP_TYPE_JPEG );
	bool WriteDataToFile( const wxString &file );

	bool Finished();
	void Abort();

	bool Ok();
	wxString GetLastError();
	
	bool IsStarted();
	
	class DLThread;
	DLThread *GetThread();
protected:
	friend class DLThread;
	DLThread *m_thread;
	wxEvtHandler *m_handler;
	int m_id;
	void (*m_callback)(wxSimpleCurlEvent &, void * );
	void *m_userData;

	wxString m_postData;
	wxArrayString m_httpHeaders;
};


#endif

