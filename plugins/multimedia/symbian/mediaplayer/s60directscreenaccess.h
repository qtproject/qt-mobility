#include <w32std.h>
#include <bitstd.h>
 
class S60DirectScreenAccess : public MDirectScreenAccess
{
public:
    static S60DirectScreenAccess* NewL(RWsSession& windowSession, CWsScreenDevice& screenDevice, RWindowBase& window);
    static S60DirectScreenAccess* NewLC(RWsSession& windowSession, CWsScreenDevice& screenDevice, RWindowBase& window);
    ~S60DirectScreenAccess();
 
    CWsScreenDevice& GetScreenDevice()
    {
        return m_screenDevice;
    }
 
    RWindowBase& GetWindow()
    {
        return m_window;
    }
 
    CFbsBitGc* GetScreenGc()
    {
        return m_gc;
    }
 
    void Start();
    void Stop();
    void Update();
 
private:
    S60DirectScreenAccess(RWsSession& windowSession, CWsScreenDevice& screenDevice, RWindowBase& window);
    void ConstructL();
    void Restart(RDirectScreenAccess::TTerminationReasons aReason);
    void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
 
private:
    CWsScreenDevice &m_screenDevice;
    RWsSession &m_windowSession;
    RWindowBase &m_window;
    CDirectScreenAccess *m_directScreenAccess;
    RRegion *m_region;
    CFbsBitGc *m_gc;
    TBool m_isDrawing;
};
