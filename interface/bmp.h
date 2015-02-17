#ifndef DEF_L_Interface_bmp
#define DEF_L_Interface_bmp

#include <L/L.h>

namespace L{
    class BMP : public Interface<Image::Bitmap>{
        public: BMP() : Interface("bmp"){}

        void to(const Image::Bitmap& bmp, std::ostream& os){
            int width = bmp.width(), height = bmp.height(), size = bmp.width()*3, lineExcess;
            lineExcess = (4-(size%4))%4;
            size+=lineExcess;
            size*=height;
            size+=54;
            Color c;

            // BITMAPFILEHEADER
                os << 'B';
                os << 'M';
                // Size of the os in bytes
                os << ultbLE(size);
                // Reserved
                os << ultbLE(0);
                // Offset of the bitmap data
                os << ultbLE(54);

            // BITMAPINFOHEADER
                // Size of the BITMAPINFOHEADER structure, in bytes
                os << ultbLE(40);
                // Width in pixel
                os << ultbLE(bmp.width());
                // Height in pixel
                os << ultbLE(bmp.height());
                // Number of planes of the target device
                os << ultbLE(1,2);
                // Number of bits per pixel
                os << ultbLE(24,2);
                // Type of compression
                os << ultbLE(0);
                // Size of the image data, in bytes
                os << ultbLE(0);
                // Horizontal pixels per meter
                os << ultbLE(0);
                // Vertical pixels per meter
                os << ultbLE(0);
                // Number of colors used in the bitmap
                os << ultbLE(0);
                // Number of color that are 'important' for the bitmap
                os << ultbLE(0);

            // PIXELDATA
                for(int y=height-1;y>=0;y--){
                    for(int x=0;x<width;x++){
                        c = bmp(x,y);
                        os << c.b() << c.g() << c.r();
                    }
                    os << Vector<byte>(lineExcess,0);
                }
        }
        void from(Image::Bitmap& bmp, const File& file){
            Vector<byte> bytes = bytesFromFile(file.gPath());
            int width, height, dataOffset, bitCount, x = 0, y;
            size_t i;

            bitCount = btulBE(SubVector(bytes,28,2));
            dataOffset = btulBE(SubVector(bytes,10,4));
            width = btulBE(SubVector(bytes,18,4));
            height = btulBE(SubVector(bytes,22,4));
            switch(bitCount){
                case 24:
                    bmp.resize(width,height);
                    i = dataOffset;
                    y = height-1;
                    while(x<bmp.width() && y<bmp.height() && i<=bytes.size()-3){
                        bmp(x,y) = Color(bytes[i+2],bytes[i+1],bytes[i]);
                        x++;i+=3;

                        if(x == width){
                            x = 0;
                            y--;
                            i+=(4-((width*3)%4))%4;
                        }
                    }
                    break;
                case 32:
                    bmp.resize(width,height);
                    i = dataOffset;
                    y = height-1;
                    while(i<=bytes.size()-4){
                        if(x == width){
                            x = 0;
                            y--;
                        }
                        bmp(x,y) = Color(bytes[i+2],bytes[i+1],bytes[i],bytes[i+3]);
                        x++;i+=4;
                    }
                    break;
                default: throw "Tried to load a BMP with unhandled bitCount.";
            }
        }
    };
}

#endif

