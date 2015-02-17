#ifndef DEF_L_Interface_png
#define DEF_L_Interface_png

#include <L/L.h>
#include <png/png.h>

namespace L{
    class PNG : public Interface<Image::Bitmap>{
        public: PNG() : Interface("png"){}

        int x, y;

        int width, height;
        png_byte color_type;
        png_byte bit_depth;

        png_structp png_ptr;
        png_infop info_ptr;
        int number_of_passes;
        png_bytep * row_pointers;

        void to(const Image::Bitmap& bmp, const File& f){
            /* create file */
            FILE *fp = fopen(f.gPath().c_str(), "wb");
            if(!fp)
                throw Exception("Couldn't open file.");

            /* initialize stuff */
            png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

            if(!png_ptr)
                throw Exception("[write_png_file] png_create_write_struct failed");

            info_ptr = png_create_info_struct(png_ptr);
            if(!info_ptr)
                throw Exception("[write_png_file] png_create_info_struct failed");

            if(setjmp(png_jmpbuf(png_ptr)))
                throw Exception("[write_png_file] Error during init_io");

            png_init_io(png_ptr, fp);

            /* write header */
            if(setjmp(png_jmpbuf(png_ptr)))
                throw Exception("[write_png_file] Error during writing header");

            png_set_IHDR(png_ptr, info_ptr, bmp.width(), bmp.height(),
                         bit_depth, color_type, PNG_INTERLACE_NONE,
                         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

            png_write_info(png_ptr, info_ptr);

            /* write bytes */
            if(setjmp(png_jmpbuf(png_ptr)))
                throw Exception("[write_png_file] Error during writing bytes");

            png_write_image(png_ptr, row_pointers);


            /* end write */
            if(setjmp(png_jmpbuf(png_ptr)))
                throw Exception("[write_png_file] Error during end of write");

            png_write_end(png_ptr, NULL);

            /* cleanup heap allocation */
            for (y=0; y<height; y++)
                    free(row_pointers[y]);
            free(row_pointers);

            fclose(fp);
        }
        void from(Image::Bitmap& bmp, const File& file){
            char header[8];    // 8 is the maximum size that can be checked

            /* open file and test for it being a png */
            FILE *fp = fopen(file.gPath().c_str(), "rb");
            if(!fp)
                throw Exception("Couldn't open file.");
            fread(header, 1, 8, fp);
            if(png_sig_cmp((byte*)header, 0, 8))
                throw Exception("This file isn't png.");


            /* initialize stuff */
            png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

            if(!png_ptr)
                throw Exception("[read_png_file] png_create_read_struct failed");

            info_ptr = png_create_info_struct(png_ptr);
            if(!info_ptr)
                throw Exception("[read_png_file] png_create_info_struct failed");

            if(setjmp(png_jmpbuf(png_ptr)))
                throw Exception("[read_png_file] Error during init_io");

            png_init_io(png_ptr, fp);
            png_set_sig_bytes(png_ptr, 8);

            png_read_info(png_ptr, info_ptr);

            width = png_get_image_width(png_ptr, info_ptr);
            height = png_get_image_height(png_ptr, info_ptr);
            color_type = png_get_color_type(png_ptr, info_ptr);
            bit_depth = png_get_bit_depth(png_ptr, info_ptr);

            number_of_passes = png_set_interlace_handling(png_ptr);
            png_read_update_info(png_ptr, info_ptr);


            /* read file */
            if(setjmp(png_jmpbuf(png_ptr)))
                throw Exception("[read_png_file] Error during read_image");

            row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
            for(y=0; y<height; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

            png_read_image(png_ptr, row_pointers);

            fclose(fp);

            // Copy data
            bmp.resize(width,height);
            if(color_type == PNG_COLOR_TYPE_RGB)
                for(y=0;y<height;y++){
                    png_byte* p = row_pointers[y];
                    for(x=0;x<width;x++){
                        bmp(x,y) = Color(p[0], p[1], p[2]);
                        p += 3;
                    }
                }
            else if(color_type == PNG_COLOR_TYPE_RGBA)
                for(y=0;y<height;y++){
                    png_byte* p = row_pointers[y];
                    for(x=0;x<width;x++){
                        bmp(x,y) = Color(p[0],p[1],p[2],p[3]);
                        p += 4;
                    }
                }
            else
                throw Exception("Unknown PNG format.");

            // Free data
            for (y=0; y<height; y++)
                    free(row_pointers[y]);
            free(row_pointers);
        }
    };
}

#endif


