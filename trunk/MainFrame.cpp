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
	panel = new wxPanel(this);

	// create viewport
	viewport = new GLViewport(panel,wxID_ANY);

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
