#include <random.h>

#if __linux__

#include <unistd.h>
#include <fcntl.h>

size_t eir_generate(eit_byte_t* dest, size_t count) {
    int fd; ssize_t ss;
    if (!dest || count == 0) return 0;
    fd = open("/dev/random", O_RDONLY);
    if (fd < 0) return 0;
    ss = read(fd, dest, count);
    if (ss < 0) ss = 0;
    close(fd);
    return ss;
}

#elif _WIN32

#error "TODO: Implement for windows"

#else
#  error "Unsupported platform"
#endif