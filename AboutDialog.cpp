#include "AboutDialog.h"


AboutDialog::AboutDialog(wxWindow *parent, wxWindowID id,
	const wxString &title, const wxPoint &position,
	const wxSize& size, long style)
	: wxDialog(parent, id, title, position, size, style)
{

	
	// put ok button
	wxButton* okButton = new wxButton(this,wxID_OK,_("Ok"),wxPoint(this->GetSize().x/2-40,140));

	// build string to be displayed on about box
	std::string aboutText = "\n\n\n"
		"Revision: " SVN_REV "\n"
		"Compiled at: " __DATE__ " " __TIME__ "\n"
		"\n"
		"Program that build 3D models based on input of the Kinect Device \n""Avaiable at:";

	// put on a static text
	wxStaticText* const Message = new wxStaticText(this, wxID_ANY,
		wxString::FromAscii(aboutText.c_str()));

	Message->Wrap(GetSize().GetWidth());

	wxStaticText* programTitle = new wxStaticText(this,wxID_ANY,_(APPNAME),wxPoint(90,10),wxDefaultSize,
													wxALIGN_CENTRE_HORIZONTAL);
	programTitle->Wrap(this->GetSize().GetWidth());

	// change font to bold
	wxFont font = programTitle->GetFont();
	font.SetPointSize(14);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	programTitle->SetFont(font);


	SetFocus();
	Center();
}



