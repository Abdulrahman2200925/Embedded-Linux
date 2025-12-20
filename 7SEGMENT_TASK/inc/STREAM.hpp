#ifndef STREAM_HPP
#define STREAM_HPP

class Stream
{
public:
    // Mandatory: virtual destructor for polymorphic deletion
    virtual ~Stream() = default;

protected:
    // Prevent direct deletion through non-polymorphic use if desired
    Stream() = default;

    // Disable copying (streams should not be copied)
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;

    // Allow moving if derived classes want it
    Stream(Stream&&) = default;
    Stream& operator=(Stream&&) = default;
};

#endif // STREAM_HPP
