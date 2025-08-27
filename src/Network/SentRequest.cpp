#include "Network/MessageType.hpp"
#include <asioexec/use_sender.hpp>
#include <stdexec/execution.hpp>

template <Network::RequestType T>
auto sendRequest(const T request,
                 std::function<void(const typename T::ResponseType)> function)
    -> stdexec::sender auto {

}
