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


BYTE* LoadPng(const char * file_name,glm::uvec2 &size,bool &hasAlpha)
{
	png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    //if (setjmp(png_jmpbuf(png_ptr))) {
    //    fprintf(stderr, "error from libpng\n");
    //    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    //    fclose(fp);
    //    return 0;
    //}

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

	if (color_type == 6)
	{hasAlpha = true;}
	else {hasAlpha = false;}

    int width = temp_width;
    int height = temp_height;

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data;
	image_data =(png_byte*)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep * row_pointers =(png_bytep*)malloc(temp_height * sizeof(png_bytep));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    int i;
    for (i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    //GLuint texture;
    //glGenTextures(1, &texture);
    //glBindTexture(GL_TEXTURE_2D, texture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(row_pointers);
    fclose(fp);

	// fill size
	size.x = temp_width;
	size.y = temp_height;

	return (BYTE*)image_data;
}

bool SavePng(const char *savefile, BYTE* data_in, int w, int h, bool hasAlpha)
{
    unsigned char *data = (unsigned char *)data_in;
    int p = 0;
    FILE *fp;

	int width, height;
	int x,y;
	png_byte color_type;
	png_byte bit_depth;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;
	int BPP;

    width = w;
    height = h;
    bit_depth = 8;

	if (hasAlpha)
	{
		color_type = 6;
		BPP = 4;
	}
	else
	{
		color_type = 2;
		BPP = 3;
	}

    // create file 
    fp = fopen(savefile, "wb");
    if(!fp) 
	{
		MessageBoxA(0,"[write_png_file] File %s could not be opened for writing.","Error",(MB_OK | MB_ICONEXCLAMATION));
		return false;
	}

    // initialize stuff 
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) 
	{
		MessageBoxA(0,"[write_png_file] png_create_write_struct failed","Error",(MB_OK | MB_ICONEXCLAMATION));
		return false;
	}

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) 
	{
		MessageBoxA(0,"[write_png_file] png_create_info_struct failed","Error",(MB_OK | MB_ICONEXCLAMATION));
		return false;
	}
    if(setjmp(png_jmpbuf(png_ptr))) 
	{
		MessageBoxA(0,"[write_png_file] Error during init_io","Error",(MB_OK | MB_ICONEXCLAMATION));
		return false;
	}

    png_init_io(png_ptr, fp);
    // write header 
    if(setjmp(png_jmpbuf(png_ptr)))
	{
		MessageBoxA(0,"[write_png_file] Error during writing header","Error",(MB_OK | MB_ICONEXCLAMATION));
		return false;
	}

    png_set_IHDR(
        png_ptr, info_ptr, width, height,
        bit_depth, color_type, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
    );

    png_write_info(png_ptr, info_ptr);

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(y = 0; y < height; y++){
        row_pointers[y] = (png_byte*)malloc(width*BPP);//png_get_rowbytes(png_ptr,info_ptr));
        for(x = 0; x < width; x++){
            row_pointers[y][x*BPP+0] = data[p++];
            row_pointers[y][x*BPP+1] = data[p++];
            row_pointers[y][x*BPP+2] = data[p++];
			row_pointers[y][x*BPP+3] = 255; p++;
        }
    }
    // write bytes 
    if(setjmp(png_jmpbuf(png_ptr))) 
	{
		MessageBoxA(0,"[write_png_file] Error during writing bytes","Error",(MB_OK | MB_ICONEXCLAMATION));
		return false;
	}

    png_write_image(png_ptr, row_pointers);

    // end write 
    if(setjmp(png_jmpbuf(png_ptr))) 
	{
		MessageBoxA(0,"[write_png_file] Error during end of write","Error",(MB_OK | MB_ICONEXCLAMATION));
		return false;
	}

    png_write_end(png_ptr, NULL);

    // cleanup heap allocation 
    for(y = 0; y < height; y++){
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fp);

    return true;
}


void BGRAtoRGBA(BYTE *buffer, int x, int y)
{
	//BYTE r,g,b;
	BYTE temp;
	int size = x * y * 4;
	for (int j = 0; j < size; j+=4)
	{
		// swap values
		temp = buffer[j];
		buffer[j] = buffer[j+2];
		buffer[j+2] = temp;
	}
}

BYTE* InvertLines(BYTE *buffer,int width, int height)
{
	int bpc = 4;
	int lineSize = width * bpc;
	BYTE* newBuffer = (BYTE*)malloc(sizeof(BYTE) * width * height * bpc);
	// swap lines only
	for (int p = 0, u = height-1; p < height-1; p++, u--)
	{
		memcpy(&(newBuffer[u * width * bpc]),&(buffer[p * width * bpc]),lineSize);
	}

	delete buffer;
	return newBuffer;
}