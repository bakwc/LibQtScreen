#pragma once

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include <streambuf>
#include <stdint.h>

template<class T>
class TSerializer {
public:
    static void Save(std::ostream& out, const T& object) {
        object.Save(out);
    }
    static void Load(std::istream& in, T& object) {
        object.Load(in);
    }
};

template <class T>
static inline void Save(std::ostream& out, const T& t);

template <class T>
static inline void Load(std::istream& in, T& t);


template <class A, class B>
class TSerializer<std::pair<A, B> > {
public:
    static void Save(std::ostream& out, const std::pair<A, B>& object) {
        ::Save(out, object.first);
        ::Save(out, object.second);
    }
    static void Load(std::istream& in, std::pair<A, B>& object) {
        ::Load(in, object.first);
        ::Load(in, object.second);
    }
};

template<class TVec, class TObj>
class TVectorSerializer {
public:
    static inline void Save(std::ostream& out, const TVec& object) {
        unsigned short size = object.size();
        out.write((const char*)(&size), 2);
        for (const auto& obj: object) {
            ::Save(out, obj);
        }
    }

    static inline void Load(std::istream& in, TVec& object) {
        unsigned short size;
        in.read((char*)(&size), 2);
        object.clear();
        for (size_t i = 0; i < size; ++i) {
            TObj obj;
            ::Load(in, obj);
            object.push_back(std::move(obj));
        }
    }
};

template <class T> class TSerializer<std::vector<T> >: public TVectorSerializer<std::vector<T>, T > {};
template <class T> class TSerializer<std::list<T> >: public TVectorSerializer<std::list<T>, T > {};
template <> class TSerializer<std::string>: public TVectorSerializer<std::string, char> {};

template <class T>
class TPodSerializer {
public:
    static inline void Save(std::ostream& out, const T& object) {
        out.write((const char*)(&object), sizeof(T));
    }
    static inline void Load(std::istream& in, T& object) {
        in.read((char*)(&object), sizeof(T));
    }
};

template <> class TSerializer<bool>: public TPodSerializer<bool> {};
template <> class TSerializer<char>: public TPodSerializer<char> {};
template <> class TSerializer<signed char>: public TPodSerializer<signed char> {};
template <> class TSerializer<uint8_t>: public TPodSerializer<uint8_t> {};
template <> class TSerializer<int16_t>: public TPodSerializer<int16_t> {};
template <> class TSerializer<uint16_t>: public TPodSerializer<uint16_t> {};
template <> class TSerializer<int32_t>: public TPodSerializer<int32_t> {};
template <> class TSerializer<uint32_t>: public TPodSerializer<uint32_t> {};
template <> class TSerializer<long>: public TPodSerializer<long> {};
template <> class TSerializer<unsigned long>: public TPodSerializer<unsigned long> {};

template <> class TSerializer<int64_t>: public TPodSerializer<int64_t> {};
template <> class TSerializer<uint64_t>: public TPodSerializer<uint64_t> {};

template <> class TSerializer<float>: public TPodSerializer<float> {};
template <> class TSerializer<double>: public TPodSerializer<double> {};

template <class T>
static inline void Save(std::ostream& out, const T& t) {
    TSerializer<T>::Save(out, t);
}

template <class T>
static inline void Load(std::istream& in, T& t) {
    TSerializer<T>::Load(in, t);
}

template <class T1>
static inline void SaveMany(std::ostream& out, const T1& t1) {
    ::Save(out, t1);
}
template <class T1>
static inline void LoadMany(std::istream& in, T1& t1) {
    ::Load(in, t1);
}

template <class T1, class T2>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2) {
    ::Save(out, t1);
    ::Save(out, t2);
}
template <class T1, class T2>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2) {
    ::Load(in, t1);
    ::Load(in, t2);
}

template <class T1, class T2, class T3>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2, const T3& t3) {
    ::Save(out, t1);
    ::Save(out, t2);
    ::Save(out, t3);
}
template <class T1, class T2, class T3>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2, T3& t3) {
    ::Load(in, t1);
    ::Load(in, t2);
    ::Load(in, t3);
}

template <class T1, class T2, class T3, class T4>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2, const T3& t3, const T4& t4) {
    ::Save(out, t1);
    ::Save(out, t2);
    ::Save(out, t3);
    ::Save(out, t4);
}
template <class T1, class T2, class T3, class T4>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2, T3& t3, T4& t4) {
    ::Load(in, t1);
    ::Load(in, t2);
    ::Load(in, t3);
    ::Load(in, t4);
}

template <class T1, class T2, class T3, class T4, class T5>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2,
                            const T3& t3, const T4& t4, const T5& t5)
{
    ::Save(out, t1);
    ::Save(out, t2);
    ::Save(out, t3);
    ::Save(out, t4);
    ::Save(out, t5);
}
template <class T1, class T2, class T3, class T4, class T5>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2, T3& t3,
                            T4& t4, T5& t5)
{
    ::Load(in, t1);
    ::Load(in, t2);
    ::Load(in, t3);
    ::Load(in, t4);
    ::Load(in, t5);
}

template <class T1, class T2, class T3, class T4, class T5, class T6>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2,
                            const T3& t3, const T4& t4, const T5& t5, const T6& t6)
{
    ::Save(out, t1);
    ::Save(out, t2);
    ::Save(out, t3);
    ::Save(out, t4);
    ::Save(out, t5);
    ::Save(out, t6);
}
template <class T1, class T2, class T3, class T4, class T5, class T6>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2, T3& t3,
                            T4& t4, T5& t5, T6& t6)
{
    ::Load(in, t1);
    ::Load(in, t2);
    ::Load(in, t3);
    ::Load(in, t4);
    ::Load(in, t5);
    ::Load(in, t6);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2,
                            const T3& t3, const T4& t4, const T5& t5, const T6& t6,
                            const T7& t7)
{
    ::Save(out, t1);
    ::Save(out, t2);
    ::Save(out, t3);
    ::Save(out, t4);
    ::Save(out, t5);
    ::Save(out, t6);
    ::Save(out, t7);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2, T3& t3,
                            T4& t4, T5& t5, T6& t6, T7& t7)
{
    ::Load(in, t1);
    ::Load(in, t2);
    ::Load(in, t3);
    ::Load(in, t4);
    ::Load(in, t5);
    ::Load(in, t6);
    ::Load(in, t7);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2,
                            const T3& t3, const T4& t4, const T5& t5, const T6& t6,
                            const T7& t7, const T8& t8)
{
    ::Save(out, t1);
    ::Save(out, t2);
    ::Save(out, t3);
    ::Save(out, t4);
    ::Save(out, t5);
    ::Save(out, t6);
    ::Save(out, t7);
    ::Save(out, t8);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2, T3& t3,
                            T4& t4, T5& t5, T6& t6, T7& t7, T8& t8)
{
    ::Load(in, t1);
    ::Load(in, t2);
    ::Load(in, t3);
    ::Load(in, t4);
    ::Load(in, t5);
    ::Load(in, t6);
    ::Load(in, t7);
    ::Load(in, t8);
}

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
static inline void SaveMany(std::ostream& out, const T1& t1, const T2& t2,
                            const T3& t3, const T4& t4, const T5& t5, const T6& t6,
                            const T7& t7, const T8& t8, const T9& t9)
{
    ::Save(out, t1);
    ::Save(out, t2);
    ::Save(out, t3);
    ::Save(out, t4);
    ::Save(out, t5);
    ::Save(out, t6);
    ::Save(out, t7);
    ::Save(out, t8);
    ::Save(out, t9);
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
static inline void LoadMany(std::istream& in, T1& t1, T2& t2, T3& t3,
                            T4& t4, T5& t5, T6& t6, T7& t7, T8& t8, T9& t9)
{
    ::Load(in, t1);
    ::Load(in, t2);
    ::Load(in, t3);
    ::Load(in, t4);
    ::Load(in, t5);
    ::Load(in, t6);
    ::Load(in, t7);
    ::Load(in, t8);
    ::Load(in, t9);
}

#define SAVELOAD(...) \
    inline virtual void Save(std::ostream& out) const { \
        ::SaveMany(out, __VA_ARGS__);             \
    } \
 \
    inline virtual void Load(std::istream& in) { \
        ::LoadMany(in, __VA_ARGS__);             \
    }


struct membuf: std::streambuf {
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};
struct imemstream: virtual membuf, std::istream {
    imemstream(char const* base, size_t size)
        : membuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};

#define SAVELOAD_POD(TypeName) template <> class TSerializer<TypeName>: public TPodSerializer<TypeName> {};
