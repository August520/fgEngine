
#include <type_traits>

namespace fg {
    namespace render {
        template <typename COMPONENT_TYPE> class OrderedCollection final {
            friend class SceneComposition;

        public:
            void resetIteration() {
                _iterator = std::begin(_elements);
                _cur = nullptr;
            }

            bool next() {
                while (_iterator != std::end(_elements)) {
                    _cur = *_iterator++;
                    
                    if (_cur->isResourcesReady(_platform, _resMan)) {
                        return true;
                    }
                }

                return false;
            }

            const COMPONENT_TYPE *get() const {
                return _cur;
            }

            unsigned count() const {
                return _elements.size();
            }

        private:
            OrderedCollection(platform::PlatformInterface &platform, resources::ResourceManagerInterface &resMan) : _platform(platform), _resMan(resMan) {
                _elements.reserve(64);
            }

            ~OrderedCollection() {

            }

            template <typename CMPF> inline void add(COMPONENT_TYPE *ptr, const CMPF &cmpf) {
                if (_elements.empty()) {
                    _elements.emplace_back(ptr);
                }
                else {
                    if (cmpf(_elements.back(), ptr)) {
                        _elements.emplace_back(ptr);
                    }
                    else {
                        size_t size = _elements.size();
                        size_t left = 0;
                        size_t right = size - 1;

                        _elements.resize(size + 1);

                        while (cmpf(_elements[left], ptr)) {
                            size_t center = (left + right) / 2;

                            if (cmpf(_elements[center], ptr)) {
                                left = center + 1;
                            }
                            else {
                                right = center;
                            }
                        }

                        COMPONENT_TYPE **elements = &_elements[0];
                        memmove(elements + left + 1, elements + left, sizeof(COMPONENT_TYPE *) * (size - left));
                        _elements[left] = ptr;
                    }
                }
            }

            template <typename CMPF> inline void remove(COMPONENT_TYPE *ptr, const CMPF &cmpf) {
                size_t size = _elements.size();
                size_t left = 0;
                size_t right = size - 1;

                while (cmpf(_elements[left], ptr)) {
                    size_t center = (left + right) / 2;

                    if (cmpf(_elements[center], ptr)) {
                        left = center + 1;
                    }
                    else {
                        right = center;
                    }
                }

                for (; left < size; left++) {
                    if (_elements[left] == ptr) {
                        COMPONENT_TYPE **elements = &_elements[0];
                        memcpy(elements + left, elements + left + 1, sizeof(COMPONENT_TYPE *) * (size - left - 1));
                        _elements.pop_back();
                        return;
                    }
                }
            }

            template <typename CMPF> inline void sort(const CMPF &cmpf) {
                if (_elements.empty() == false) {
                    COMPONENT_TYPE **ptr = &_elements[0];

                    for (int i = 1, c = 0, cnt = int(_elements.size()); i < cnt; c = i, i++) {
                        auto key = *(ptr + i);
                        while (c >= 0 && cmpf(key, *(ptr + c))) {
                            c--;
                        }

                        memmove(ptr + c + 2, ptr + c + 1, sizeof(COMPONENT_TYPE *) * (i - c - 1));
                        _elements[c + 1] = key;
                    }
                }
            }

            platform::PlatformInterface &_platform;
            resources::ResourceManagerInterface &_resMan;
            
            std::vector <COMPONENT_TYPE *> _elements;
            typename std::vector <COMPONENT_TYPE *> ::iterator _iterator = std::begin(_elements);
            COMPONENT_TYPE *_cur = nullptr;
        };
    }
}