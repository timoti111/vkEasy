#include <iostream>
#include <vkEasy/Context.h>

int main()
{
    try {
        vk::easy::Context::initialize();
    } catch (vk::SystemError& err) {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    } catch (vk::easy::Exception& err) {
        std::cout << "vk::easy:Exception: " << err.what() << std::endl;
        exit(-1);
    } catch (std::exception& err) {
        std::cout << "std::exception: " << err.what() << std::endl;
        exit(-1);
    } catch (...) {
        std::cout << "unknown error\n";
        exit(-1);
    }
    return 0;
}
