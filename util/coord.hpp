#pragma once

#include <ostream>
#include <array>

namespace util
{
    template<typename dimen_t = int>
    struct _coord2D{
        typedef dimen_t type;
        dimen_t mX, mY;

        inline void operator+=(const _coord2D<dimen_t>& to_add) {
            this->mX += to_add.mX;
            this->mY += to_add.mY;
        }

        inline _coord2D<dimen_t> operator+(const std::array<dimen_t, 2>& to_add) const {
            return {
                this->mX + to_add[0],
                this->mY + to_add[1],
            };
        }

        inline void operator+=(const std::array<dimen_t,2>& to_add) {
            this->mX += to_add[0];
            this->mY += to_add[1];
        }

        //inline bool operator==(const _coord<dimen_t>& second) const = default;  // requires C++20
        inline bool operator==(const _coord2D<dimen_t>& second) const {
            return (this->mX == second.mX) &&
                    (this->mY == second.mY);
        }

        inline bool operator<(const _coord2D<dimen_t>& second) const {
            if (this->mX < second.mX)  return true;
            else return (this->mY < second.mY);
        }

        inline bool operator>(const _coord2D<dimen_t>& second) const {
            return !this->operator==(second) && !this->operator<(second);
        }

        friend std::ostream& operator<<(std::ostream& os, const _coord2D<dimen_t>& coord){
            if(coord.mX >= 0){
                os << ' ';
                if(coord.mX < 10)
                    os << ' ';
            }else if(coord.mX > -10)
                os << ' ';
            os << coord.mX <<',';

            if(coord.mY >= 0){
                os << ' ';
                if(coord.mY < 10)
                    os << ' ';
            }else if(coord.mY > -10)
                os << ' ';
            os << coord.mY << ' ';

            return os;
        }

        _coord2D() noexcept : _coord2D(dimen_t{}, dimen_t{}) {}
        _coord2D(dimen_t x, dimen_t y) noexcept : mX(x), mY(y){}
    };
} // namespace util
