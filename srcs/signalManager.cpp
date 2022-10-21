#include "../incs/signalManager.hpp"

int setGiveSocket(int valSocket)
{
    static int serverSocket;
    if (valSocket)
        serverSocket = valSocket;
    return (serverSocket);
}

void    signalHandler(int sig, siginfo_t *siginfo, void *ucontext)
{
    (void)sig;
    (void)siginfo;
    (void)ucontext;
    close(setGiveSocket(0));
    _exit(sig);
}

void    signalOn(int servSocket)
{
    setGiveSocket(servSocket);
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGKILL);
    act.sa_sigaction = signalHandler;
    sigaction(SIGINT, &act, NULL);
}