
class CFixHtmlView : public CHtmlView
{

public:
    CString GetFullName() const;
    CString GetType() const;
    CString GetLocationName() const;
    CString GetLocationURL() const;

    void Navigate(LPCTSTR lpszURL, DWORD dwFlags = 0 ,  
                  LPCTSTR lpszTargetFrameName = NULL ,  
                  LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,  
                  DWORD dwPostDataLen = 0);

    BOOL LoadFromResource(LPCTSTR lpszResource);
    BOOL LoadFromResource(UINT nRes);
};