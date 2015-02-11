
namespace fg {
    namespace resources {
        AnimationResource::BoneAnimation::BoneAnimation(unsigned keyCount){
            animKeyCount = keyCount;
            animationKeys = new AnimKey [keyCount];
        }

        AnimationResource::BoneAnimation::~BoneAnimation(){
            delete [] animationKeys;
        }

        AnimationResource::~AnimationResource() {
            _boneAnims.foreach([](const fg::string &key, const BoneAnimation *ptr){
                delete ptr;
            });
        }

        void AnimationResource::loaded(const diag::LogInterface &log) {
            byteform data (_binaryData, _binarySize);

            unsigned signature = data.readDword();
            unsigned flags = data.readDword();
            unsigned objCount = data.readDword();

            for(unsigned c = 0; c < objCount; c++){
                char     tname[32];
                unsigned tkeyCount;

                data.readString(tname);
                data.readDword(tkeyCount);

                BoneAnimation *boneAnim = new BoneAnimation(tkeyCount);
                _boneAnims.add(tname, boneAnim);

                for(unsigned i = 0; i < tkeyCount; i++){
                    boneAnim->animationKeys[i].localRotation.x = data.readFloat();
                    boneAnim->animationKeys[i].localRotation.y = data.readFloat();
                    boneAnim->animationKeys[i].localRotation.z = data.readFloat();
                    boneAnim->animationKeys[i].localRotation.w = data.readFloat();
                    boneAnim->animationKeys[i].localRotation.normalize();
                    
                    boneAnim->animationKeys[i].localScale.x = data.readFloat();
                    boneAnim->animationKeys[i].localScale.y = data.readFloat();
                    boneAnim->animationKeys[i].localScale.z = data.readFloat();

                    boneAnim->animationKeys[i].localPosition.x = data.readFloat();
                    boneAnim->animationKeys[i].localPosition.y = data.readFloat();
                    boneAnim->animationKeys[i].localPosition.z = data.readFloat();
                }
            }
        }

        bool AnimationResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            return false;
        }

        void AnimationResource::unloaded() {
            _boneAnims.foreach([](const fg::string &key, const BoneAnimation *ptr) {
                delete ptr;
            });

            _boneAnims.clear();
        }

        bool AnimationResource::getMatrix(const fg::string &boneName, float animKoeff, math::m4x4 &oMatrix) const {
            auto ptr = _boneAnims.get(boneName);

            if(ptr != nullptr){  
                const BoneAnimation &cur = *ptr; 

                unsigned lineSize = cur.animKeyCount;
                float    koeff = float(lineSize) * animKoeff;

                unsigned key1 = unsigned(koeff);
                unsigned key2 = key1 + 1;
                float    between = koeff - float(key1);

                if(key2 >= lineSize){
                    key2 = 0;
                }

                math::quat trot;
                math::p3d  tpos;
                math::p3d  tscl;

                trot.slerp(cur.animationKeys[key1].localRotation, cur.animationKeys[key2].localRotation, between);
                tpos = cur.animationKeys[key1].localPosition + (cur.animationKeys[key2].localPosition - cur.animationKeys[key1].localPosition) * between;
                tscl = cur.animationKeys[key1].localScale + (cur.animationKeys[key2].localScale - cur.animationKeys[key1].localScale) * between;

                trot.toMatrix(oMatrix);
                (*(math::p3d *)&oMatrix._11).toLength(tscl.x);
                (*(math::p3d *)&oMatrix._21).toLength(tscl.y);
                (*(math::p3d *)&oMatrix._31).toLength(tscl.z);

                oMatrix._41 = tpos.x;
                oMatrix._42 = tpos.y;
                oMatrix._43 = tpos.z;

                return true;
            }

            return false;
        }

        bool AnimationResource::getTransform(const fg::string &boneName, float animKoeff, math::p3d &oTranslation, math::quat &oRotation, math::p3d &oScaling) const {
            auto ptr = _boneAnims.get(boneName);

            if(ptr != nullptr){
                const BoneAnimation &cur = *ptr;

                unsigned lineSize = cur.animKeyCount;
                float    koeff = float(lineSize) * animKoeff;

                unsigned key1 = unsigned(koeff);
                unsigned key2 = key1 + 1;
                float    between = koeff - float(key1);

                if(key2 >= lineSize){
                    key2 = 0;
                }

                oRotation.slerp(cur.animationKeys[key1].localRotation, cur.animationKeys[key2].localRotation, between);
                oTranslation = cur.animationKeys[key1].localPosition + (cur.animationKeys[key2].localPosition - cur.animationKeys[key1].localPosition) * between;
                oScaling = cur.animationKeys[key1].localScale + (cur.animationKeys[key2].localScale - cur.animationKeys[key1].localScale) * between;
                return true;
            }

            return false;
        }
    }
}




