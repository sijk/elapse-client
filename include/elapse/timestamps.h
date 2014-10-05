#ifndef TIMESTAMPS_H
#define TIMESTAMPS_H

#include <cstdint>
#include <ctime>
#include <map>
#include <vector>
#include <QString>

namespace elapse {

/*!
 * \brief Utilities for working with time.
 */
namespace time {

/*!
 * time::Point%s are expressed in nanoseconds since the Unix epoch.
 *
 * This won't overflow until December 2554.
 */
typedef std::uint64_t Point;

/*!
 * \brief User-defined literals for expressing time::Point%s.
 *
 *     1_us == 1000_ns == 1000
 */
namespace literals {

constexpr time::Point operator"" _ns(unsigned long long n) { return n; }
constexpr time::Point operator"" _us(unsigned long long n) { return n * 1000_ns; }
constexpr time::Point operator"" _ms(unsigned long long n) { return n * 1000_us; }
constexpr time::Point operator"" _s (unsigned long long n) { return n * 1000_ms; }

} // namespace literals


template<typename T>
constexpr time::Point from_us(T t)
{
    using namespace literals;
    return t * 1_us;
}

template<typename T>
constexpr time::Point from_ms(T t)
{
    using namespace literals;
    return t * 1_ms;
}

template<typename T>
constexpr time::Point from_s(T t)
{
    using namespace literals;
    return t * 1_s;
}

inline time::Point from_timespec(const struct timespec *ts)
{
    return from_s(ts->tv_sec) + ts->tv_nsec;
}

/*!
 * \return the \a time in ISO 8601 format (yyyy-mm-ddThh:mm:ss.fffffffff).
 */
inline QString format(time::Point time)
{
    using namespace literals;

    const std::time_t seconds = time / 1_s;
    char date[20];
    std::strftime(date, 20, "%FT%T", std::gmtime(&seconds));

    return QString().sprintf("%s.%09lu", date, time % 1_s);
}

/*!
 * \brief The time::Series class maps time::Point%s to arbitrary values.
 *
 * This class exists primarily as a convenience for implementing
 * FeatureExtractor%s. It provides an easy way to remove all values
 * which have time::Point%s less than a given value.
 *
 * \headerfile elapse/timestamps.h
 */
template<typename T>
class Series : public std::map<time::Point, T>
{
public:
    /*!
     * Remove all values from time::Point%s earlier than the given \a time.
     *
     * Relies on the fact that std::map%s are sorted by their keys.
     */
    void removeValuesBefore(time::Point time)
    {
        auto i = this->begin();
        auto end = this->lower_bound(time);

        while (i != end)
            i = this->erase(i);
    }

    /*!
     * \return the values as a std::vector.
     */
    std::vector<T> values() const
    {
        std::vector<T> vals;
        vals.reserve(this->size());
        for (auto value : *this)
            vals.push_back(value.second);
        return vals;
    }
};

} // namespace time

} // namespace elapse


inline QDataStream &operator>>(QDataStream &stream, elapse::time::Point &value)
{
    return stream >> reinterpret_cast<quint64&>(value);
}

#endif // TIMESTAMPS_H
