#include "s60directscreenaccess.h"
 
 
S60DirectScreenAccess* S60DirectScreenAccess::NewL(RWsSession& windowSession, CWsScreenDevice& screenDevice, RWindowBase& window)
{
    S60DirectScreenAccess* self = S60DirectScreenAccess::NewLC(windowSession, screenDevice, window);
	CleanupStack::Pop(self);
	return self;
}
 
S60DirectScreenAccess* S60DirectScreenAccess::NewLC(RWsSession& windowSession, CWsScreenDevice& screenDevice, RWindowBase& window)
{
    S60DirectScreenAccess* self = new (ELeave) S60DirectScreenAccess(windowSession, screenDevice, window);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}
 
S60DirectScreenAccess::S60DirectScreenAccess(RWsSession& windowSession, CWsScreenDevice& screenDevice, RWindowBase& window):
    m_windowSession(windowSession),
    m_screenDevice(screenDevice),
    m_window(window),
    m_directScreenAccess(NULL),
    m_region(NULL),
    m_gc(NULL),
    m_isDrawing(EFalse)
{
}
 
S60DirectScreenAccess::~S60DirectScreenAccess()
{
	delete m_directScreenAccess;
}
 
void S60DirectScreenAccess::ConstructL()
{
    m_directScreenAccess = CDirectScreenAccess::NewL(m_windowSession, m_screenDevice, m_window, *this);
}
 
void S60DirectScreenAccess::Start()
{
	if(!m_isDrawing)
	{
		TRAPD(err, m_directScreenAccess->StartL());
		if(err == KErrNone)
		{
            m_gc = m_directScreenAccess->Gc();
            m_region = m_directScreenAccess->DrawingRegion();
			m_gc->SetClippingRegion(m_region);
			m_isDrawing = ETrue;
		}
	}
}
 
void S60DirectScreenAccess::Stop()
{
	if(m_isDrawing)
	{
        m_isDrawing = EFalse;
	}
}
 
void S60DirectScreenAccess::Update()
{
    m_directScreenAccess->ScreenDevice()->Update();
}
 
void S60DirectScreenAccess::Restart(RDirectScreenAccess::TTerminationReasons /*aReason*/)
{
	Start();
}
 
void S60DirectScreenAccess::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReason*/)
{
	m_directScreenAccess->Cancel();
	m_isDrawing = EFalse;
}
