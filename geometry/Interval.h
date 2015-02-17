#ifndef DEF_L_Interval
#define DEF_L_Interval

#include "../general.h"
#include "Point.h"

namespace L{
    template <int d,class T>
    class Interval{
        protected:
            Point<d,T> a, b;
            Interval(){}
        public:
            Interval(const Point<d,T>& a, const Point<d,T>& b){
                this->a = this->b = a;
                add(b);
            }

            Interval operator*(const Interval& other) const{
                Point<d,T> wtra, wtrb;
                Interval wtr;
                for(size_t i(0);i<d;i++){
                    wtra[i] = std::max(a[i],other.a[i]);
                    wtrb[i] = std::min(b[i],other.b[i]);
                }
                wtr.a = wtra;
                wtr.b = wtrb;
                return wtr;
            }

            void add(const Point<d,T>& p){
                for(size_t i(0);i<d;i++){
                    if(a[i] > p[i])
                        a[i] = p[i];
                    if(b[i] < p[i])
                        b[i] = p[i];
                }
            }
            bool contains(Point<d,T> p) const{
                if(empty()) return false;
                bool wtr(true);
                for(size_t i(0);i<d&&wtr;i++)
                    if(!InBetween(p[i],a[i],b[i]))
                        wtr = false;
                return wtr;
            }
            Point<d,T> closestTo(Point<d,T> p) const{
                for(size_t i(0);i<d;i++){
                    if(p[i]<a[i])
                        p[i] = a[i];
                    if(p[i]>b[i])
                        p[i] = b[i];
                }
                return p;
            }
            Point<d,T> size() const{
                return b - a;
            }
            bool empty() const{
                for(size_t i(0);i<d;i++)
                    if(a[i]>=b[i])
                        return true;
                return false;
            }

            inline const Point<d,T>& gA() const{return a;}
            inline const Point<d,T>& gB() const{return b;}
    };
    typedef Interval<2,int> Interval2i;
}

#endif
