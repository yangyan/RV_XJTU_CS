#include    "ProgramThreadFactory.hpp"
#include    "EventDispatcher.hpp"

#include    "Receiver.hpp"

#include    "../../modules/solution/Result.hpp"
#include    "../../modules/solution/Solution.hpp"

using namespace std;
using namespace rv_xjtu_yangyan;

int main(int argc, char *argv[])
{
    /*
     *读取所有规则文件
     */
    ProgramThreadFactory pt;
    //添加文件:可以使用循环添加文件
    pt.addFile("test.r");

    /*
     *运行所有自动机
     */
    pt.runAll();

    /*
     *给所有自动机安插分发器
     */
    EventDispatcher ed;
    ed.setThreads(pt);

    /*
     *从网络读取事件，并将事件分发
     */
    Receiver r(ed);
    while(true)
    {
        r.readAndProcess();
    }
    //由于有无限循环的存在，因此这里不能执行到
    return 0;
}
