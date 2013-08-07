#include "MainFrame.h"

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	:wxFrame(NULL, wxID_ANY, title, pos, size)
{
	CreateStatusBar();

	// help menu
	wxMenu* helpMenu = new wxMenu;
	this->Connect( K_About,  wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnAbout));
	helpMenu->Append(K_About,_("&About\tF1"),_("About this program"));

	// create top menu bar
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(helpMenu,_("Help"));

	// put the menubar on this frame
	this->SetMenuBar(menuBar);

	SetStatusText(_("Kinect3DBuilder ready!"));
	panel = new wxPanel(this);
	
	// buttons
	wxButton* load_button = new wxButton(panel,wxID_OPEN,_("Load Image from File"),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT,
										wxDefaultValidator,_("Load previously saved PNG image along with the depth file"));
	wxButton* connectk_button = new wxButton(panel,wxID_ANY,_("Start Kinect Device"),wxDefaultPosition,wxDefaultSize,
											wxBU_EXACTFIT,wxDefaultValidator);
	
	// put them on a sizer
	wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(load_button,2);
	sizer->Add(connectk_button,2);

	panel->SetSizer(sizer);
	//SetBackgroundColour(wxColour(255,255,255));
	
	// connect buttons to functionality
	//load_button->Connect(

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
