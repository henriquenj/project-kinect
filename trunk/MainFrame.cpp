#include "MainFrame.h"

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	:wxFrame(NULL, wxID_ANY, title, pos, size)
{
	CreateStatusBar();

	// help menu
	wxMenu* helpMenu = new wxMenu;
	this->Connect( K_About,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAbout));
	helpMenu->Append(K_About,_("&About\tF1"),_("About this program"));
	// file menu
	wxMenu* fileMenu = new wxMenu;
	this->Connect(K_Quit,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(MainFrame::OnQuit));
	fileMenu->Append(K_Quit,_("&Quit"),_("Quit the program"));

	// create top menu bar
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(fileMenu,_("File"));
	menuBar->Append(helpMenu,_("Help"));
	
	// put the menubar on this frame
	this->SetMenuBar(menuBar);

	SetStatusText(_("Kinect3DBuilder ready!"));
	panel = new PanelMainFrame(this,size);

	// set minimal size
	this->SetMinSize(wxSize(600,450));
	
}

MainFrame::~MainFrame()
{
}

bool MainFrame::OnInit()
{
	return true;
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	 // create about dialog
	AboutDialog a_dialog(this,wxID_ANY,wxString::FromUTF8(APPNAME),wxDefaultPosition,wxSize(340,200));

	a_dialog.ShowModal();

}

void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	this->Close();
}



//PAINEL CLASS GOES HERE

PanelMainFrame::PanelMainFrame(wxWindow* parent,wxSize size) 
	:wxPanel(parent,wxID_ANY,wxDefaultPosition,size)
{
	viewport = NULL;
	this->Connect(wxID_ANY,wxEVT_SIZE,wxSizeEventHandler(PanelMainFrame::OnSize));

	// this window size
	wxSize frameSize = this->GetSize();
	// create viewport
	viewport = new GLViewport(this,wxID_ANY,wxDefaultPosition,wxSize(735,515));

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	
	//sizer->Add(viewport);
	this->SetSizer(sizer);

	// setloop mode
	viewport->ChangeLoopMode(false);

	/*
	TEMPORARY STUFF
	*/
	AppConfig::SetKinectConnected(false);

	// temporary stuff ends here
}

void PanelMainFrame::OnSize(wxSizeEvent& event)
{
	// we need to resize the viewport
	wxPanel::OnSize(event);
	if (viewport != NULL)
	{
		viewport->SetSize(event.GetSize().x/1.2,event.GetSize().y/1.2);
	}
}