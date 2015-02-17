#ifndef DEF_L_TargetPRS
#define DEF_L_TargetPRS

#include "../containers/Ref.h"
#include "PRS.h"

#define L_APRSMODE_NONE         0
#define L_APRSMODE_PTARGET      1
#define L_APRSMODE_VTARGET      2
#define L_APRSMODE_THETALIMIT   4
#define L_APRSMODE_PHILIMIT     8
#define L_APRSMODE_RHOLIMIT     16

#define L_APRSSPEED_NONE        0x100
#define L_APRSSPEED_INSTANT     0x200
#define L_APRSSPEED_CONSTANT    0x300
#define L_APRSSPEED_LINEAR      0x400

// Z is up, Y is forward, X is right
namespace L{
    class TargetPRS : public PRS{
        protected:
            Ref<PRS> posTarget, viewTarget;
            Point<3,double> tPosition, tRotation, tScale,
                            posShift, viewPosShift, rotShift;
            float posSpeed, rotSpeed,
                  posThetaMin, posThetaMax,
                  posPhiMin, posPhiMax,
                  posRhoMin, posRhoMax;
            size_t modes;

        public:
            TargetPRS();
            /*
            void nextFrame(double span);
            void sModes(unsigned long modes);

            void sTarget(L_PRS* target);
            void sTarget(L_PRS* target, L_PRS* viewTarget);

            void sPosShift(L_3dD posShift);
            void sViewPosShift(L_3dD viewPosShift);
            void sRotShift(L_3dD rotShift);

            void sThetaLimits(float posThetaMin, float posThetaMax);
            void sRhoLimits(float posRhoMin, float posRhoMax);
            void sPhiLimits(float posPhiMin, float posPhiMax);

            void sSpeed(float positionSpeed, float rotationSpeed);
            */

    };
}

#endif






