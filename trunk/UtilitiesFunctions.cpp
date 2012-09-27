// define functions here to prevent link errors


#include "UtilitiesFunctions.h"
// for the documentation, see UtilitiesFunction.h

const char * ShowFileDialog(void * window, int type, const char * filterDesc, const char * extFilter)
{
#if defined (_WIN32)

	// Reused From The Tile Map Editor...

	BOOL result;
	OPENFILENAMEA open_params;
	memset(&open_params, 0, sizeof(OPENFILENAMEA));

	// This should always use thread local storage.
	static char full_file_name[MAX_PATH];

	// Clear full_file_name every call:
	memset(full_file_name, 0, MAX_PATH);

	char * ext = const_cast<char *>(extFilter);
	char * filter = const_cast<char *>(filterDesc);

	char sorted_filter[4096]; // FIXME: Make buffer size safe some day.
	char * sorted_ptr = sorted_filter;

	for (;;) // Sort the seach filter:
	{
		while ((*filter != '|') && (*filter != '\0'))
			*sorted_ptr++ = *filter++;

		while (*(sorted_ptr - 1) == ' ') // Remove white spaces.
			--sorted_ptr;

		*sorted_ptr++ = '\0';

		while ((*ext != '|') && (*ext != '\0'))
			*sorted_ptr++ = *ext++;

		while (*(sorted_ptr - 1) == ' ') // Remove white spaces.
			--sorted_ptr;

		*sorted_ptr++ = '\0';

		if ((*filter == '|') || (*ext == '|'))
		{
			// Skip white spaces:

			while (*(++filter) == ' ')
				;
			while (*(++ext) == ' ')
				;
		}
		else
		{
			*sorted_ptr++ = '\0';
			break;
		}
	}

	// Make the system call:
	open_params.lStructSize = sizeof(OPENFILENAMEA);
	open_params.hwndOwner = reinterpret_cast<HWND>(window);
	open_params.lpstrFilter = sorted_filter;
	open_params.lpstrFile = full_file_name;
	open_params.nMaxFile = MAX_PATH;
	open_params.Flags = (OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY);

	if (type == DialogSave)
		result = GetSaveFileNameA(&open_params);

	else if (type == DialogOpen)
		result = GetOpenFileNameA(&open_params);

	else result = FALSE;

	return ((result == FALSE) ? 0 : full_file_name);

#else
#	error ShowFileDialog() not implemented on this platform.
#endif
}