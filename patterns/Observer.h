#ifndef DEF_L_Observer
#define DEF_L_Observer

namespace L{
    class Observable;
    class Observer{
        private:
            Set<Observable*> observed;

        public:
            void notification(Observable*, int msg);

        friend Observable;
    };
}

#endif


