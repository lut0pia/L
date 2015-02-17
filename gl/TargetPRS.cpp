#include "TargetPRS.h"

/*
AdvancedPRS::AdvancedPRS(){
    reset();
    tPosition = position;
    tRotation = rotation;
    tScale = scale;
    posShift = {0,0,0};
    viewPosShift = {0,0,0};
    rotShift = {0,0,0};
}
void AdvancedPRS::nextFrame(double span){
    if(modes){
        3dsD ttp;
        // First calculations of the target position depending on the APRSMODE
        if(modes&APRSMODE_PTARGET){ // Mode : calculate target position using target PRS
                tPosition = posTarget->gPosition() + posShift;
                ttp = CartToSpher(position - tPosition);

                // Check for limits (regarding position relative to target)
                if(modes&APRSMODE_THETALIMIT){ // Theta limits
                    if(ttp.theta<posThetaMin)
                        ttp.theta = posThetaMin;
                    else if(ttp.theta>posThetaMax)
                        ttp.theta = posThetaMax;
                }
                if(modes&APRSMODE_PHILIMIT){ // Phi limits
                    if(ttp.phi<posPhiMin)
                        ttp.phi = posPhiMin;
                    else if(ttp.phi>posPhiMax)
                        ttp.phi = posPhiMax;
                }
                if(modes&APRSMODE_RHOLIMIT){ // Rho limits
                    if(ttp.rho<posRhoMin)
                        ttp.rho = posRhoMin;
                    else if(ttp.rho>posRhoMax)
                        ttp.rho = posRhoMax;
                }

                tPosition = tPosition + SpherToCart(ttp);
        }
        // Then calculations of the target rotation depending on the APRSMODE
        if(modes&APRSMODE_VTARGET){ // Mode : calculate rotation to face the viewTarget PRS
            ttp = CartToSpher(position - viewTarget->gPosition() + viewPosShift);

            tRotation.x = PMod(rotShift.x+ttp.theta-90,360.0);
            tRotation.y = rotShift.y;
            tRotation.z = PMod(rotShift.z+ttp.phi+90,360.0);
        }
        // Then change actual position and rotation depending on the APRSSPEED
        switch(modes&0x00000F00){
            case APRSSPEED_INSTANT:
                position = tPosition;
                rotation = tRotation;
                break;
            case APRSSPEED_CONSTANT:
                    // Sign * min(absoluteDifference,maximumMovement)
                move(((tPosition.x - position.x < 0) ? -1 : 1) * min(fabs(tPosition.x - position.x),(posSpeed*span)),
                     ((tPosition.y - position.y < 0) ? -1 : 1) * min(fabs(tPosition.y - position.y),(posSpeed*span)),
                     ((tPosition.z - position.z < 0) ? -1 : 1) * min(fabs(tPosition.z - position.z),(posSpeed*span)));
                rotate(((PModDiff(tRotation.x,rotation.x,360.0) < 0) ? -1 : 1) * min((double)fabs(PModDiff(tRotation.x,rotation.x,360.0)),(rotSpeed*span)),
                       ((PModDiff(tRotation.y,rotation.y,360.0) < 0) ? -1 : 1) * min((double)fabs(PModDiff(tRotation.y,rotation.y,360.0)),(rotSpeed*span)),
                       ((PModDiff(tRotation.z,rotation.z,360.0) < 0) ? -1 : 1) * min((double)fabs(PModDiff(tRotation.z,rotation.z,360.0)),(rotSpeed*span)));
                break;
            case APRSSPEED_LINEAR:
                move((tPosition.x - position.x) * (1-pow(1-posSpeed,span)),
                     (tPosition.y - position.y) * (1-pow(1-posSpeed,span)),
                     (tPosition.z - position.z) * (1-pow(1-posSpeed,span)));
                rotate(PModDiff(tRotation.x,rotation.x,360.0) * (1-pow(1-rotSpeed,span)),
                       PModDiff(tRotation.y,rotation.y,360.0) * (1-pow(1-rotSpeed,span)),
                       PModDiff(tRotation.z,rotation.z,360.0) * (1-pow(1-rotSpeed,span)));
                break;
            default: break;
        }
    }
}
void AdvancedPRS::sModes(unsigned long m){
    modes = m;
}

void AdvancedPRS::sTarget(PRS* t){
    posTarget = t;
    viewTarget = t;
}
void AdvancedPRS::sTarget(PRS* t, PRS* vt){
    posTarget = t;
    viewTarget = vt;
}
void AdvancedPRS::sPosShift(3dD ps){
    posShift = ps;
}
void AdvancedPRS::sViewPosShift(3dD vps){
    viewPosShift = vps;
}
void AdvancedPRS::sRotShift(3dD rs){
    rotShift = rs;
}

void AdvancedPRS::sThetaLimits(float ptmin, float ptmax){
    posThetaMin = ptmin;
    posThetaMax = ptmax;
}
void AdvancedPRS::sRhoLimits(float prmin, float prmax){
    posRhoMin = prmin;
    posRhoMax = prmax;
}
void AdvancedPRS::sPhiLimits(float ppmin, float ppmax){
    posPhiMin = ppmin;
    posPhiMax = ppmax;
}

void AdvancedPRS::sSpeed(float positionSpeed, float rotationSpeed){
    posSpeed = positionSpeed;
    rotSpeed = rotationSpeed;
}
*/
