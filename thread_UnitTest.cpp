// thread_UnitTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#ifdef DEBUG

#endif // DEBUG


#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <list>

/// <summary>
/// 无参数的方法作为线程函数。
/// </summary>
void funThread_hello() {
    std::cout << "1. 运行无参数线程"  << std::endl;
    for (size_t i = 0; i < 3; i++)
    {
        std::cout << "say hollow " << i + 1 << " times." << std::endl;
    }
}

void funThreadTerminate()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "test thread terminate timing, finished." <<" \n";

    
}

void funThread_arg1(int count) {
    std::cout << "2. 运行1个参数线程" << std::endl;
    printf("Test thread with args: %d\n", count);
}

void funThread_add(int a, int b) {
    std::cout << "3. 运行2个参数线程" << std::endl;
    std::cout << "Test Thread add " << a << " + " << b << " = " << a + b << std::endl;
}

void funThread_double(int o, int& result) {
    std::cout << "4. 运行引用参数线程" << std::endl;
    result = 2 * o;
    
}

class DataThread
{
public:
    /// 互斥锁 用于线程之间同步数据
    std::mutex mtx;
    std::list<double> dataList;
    bool rev_flags = false;
    bool snd_flags = false;

    ///  接收线程或读线程
    void funRev() {
        while (rev_flags) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            if ( dataList.size() > 0 && mtx.try_lock() ) {

                dataList.pop_front();

                std::cout << " 读取队列，长度 " << dataList.size() << " 当前数值 " << std::endl;
                /// 操作部分
                mtx.unlock();
            }
        }
    }

    /// 发送线程或写线程
    void funSend() {
        double rad = 0;
        while (snd_flags) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (mtx.try_lock()) {
                rad += 0.03;
                float tempoff = 3 * sin(rad);

                dataList.push_back(tempoff);
                std::cout << " 插入队列，长度 " << dataList.size() << " 当前数值 "<< tempoff << std::endl;
                /// 操作部分
                mtx.unlock();
            }
        }
    }

    std::thread revThread() {
        this->rev_flags = true;
        return std::thread(&DataThread::funRev, this);
    }
    std::thread sndThread() {
        this->snd_flags = true;
        return std::thread(&DataThread::funSend, this);
    }

private:

};




/// <summary>
/// 对象里的函数作为线程使用
/// </summary>
class foo {
public:
    void member_func1() {
        std::cout << "5.1 运行对象函数member_func1()线程" << std::endl;
        for (int i = 0; i < 12; ++i) {
            std::cout << "Thread bar() from foo executing " << this->n <<std::endl;
            ++n;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void member_func2(int add) {
        std::cout << "5.2 运行对象函数member_func2(int add )线程" << std::endl;
        n += add;
    }


    int n = 0;
};


class ThreadWrapper {
public:
    void member1() {
        std::cout << "6. 运行member1" << std::endl;
        double rad = 0;
        while (this->thread1_flags)
        {
            n += 1;
            std::cout << "  nd = " << n << std::endl;
        }
        std::cout << "  member1 has terminated." << std::endl;
    }
    void member2(const char* arg1, unsigned arg2) {
        std::cout << "6. member2 and my first arg is (" << arg1 << ") and second arg is (" << arg2 << ")" << std::endl;
        double rad = 0;
        while (this->thread2_flags)
        {
            rad += 0.03;
            float tempoff = arg2 * sin(rad);
            nd = tempoff;
            std::cout << "  nd = " << nd << std::endl;
        }
        std::cout << "  member2 has terminated." << std::endl;
    }
    std::thread member1Thread() {
        thread1_flags = true;
        return std::thread(&ThreadWrapper::member1, this);
    }
    std::thread member2Thread(const char* arg1, unsigned arg2) {
        this->thread2_flags = true;
        return std::thread(&ThreadWrapper::member2, this, arg1, arg2);
    }

    bool thread1_flags = false;
    bool thread2_flags = false;
    int n = 0;
    float nd = 0.0;
};

int main()
{
    std::cout << "This is a thread test !\n";
    std::cout << "主线程: " << std::this_thread::get_id() << std::endl;
    
    // 1.无参数线程
    std::thread t0(funThread_hello);
    t0.join();


    // 2.直接传递参数
    std::thread t1(funThread_arg1, 11);
    t1.join();


    // 3.线程传递多个参数 
    std::thread t2(funThread_add, 20, 50);
    t2.join();


    // 4.按引用传递。
    int res=0;
    std::thread t3(funThread_double, 14, std::ref(res));
    t3.join();
    std::cout << "Test Thread double by reference, input * 2 =" << res << std::endl;


    // 5.运行对象的函数作为线程启动
    foo f;
    // 5.1 测试是否修改了对象里的数据
    std::thread t4(&foo::member_func1, &f);
    t4.join();
    std::cout << "5.2 已修改对象foo的属性n = " << f.n << std::endl;
    // 5.2 测试带函数对象成员函数
    std::thread t41(&foo::member_func2, &f, 13);
    t41.join();
    std::cout << "5.2 带参数成员函数修改foo的属性n = " << f.n << std::endl;
    

    // 6.1 从对象内部启动和管理线程。
    ThreadWrapper* w = new ThreadWrapper();
    std::thread tw1 = w->member1Thread();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    w->thread1_flags = false;
    tw1.join();
    std::cout << " 6.1 join()等待线程，修改wrapper的属性n = " << w->n << std::endl;
    // 6.2 从对象内部启动和管理线程。detach 0.2s后关闭。
    w->member2Thread("hello", 3).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    w->thread2_flags = false;
    std::cout << " 6.2 detach()关闭线程，修改wrapper的属性nd = " << w->nd << std::endl;


    // 7. 多线程间通信，用mutex互锁。
    DataThread dt;
    dt.sndThread().detach();
    dt.revThread().detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    dt.rev_flags = false;
    dt.snd_flags = false;


    // 4.测试退出。开启一个延迟5s关闭的线程，主线程延迟1s。
    //   测试join和detach和主线程的退出时间的关系。 
    std::thread t(funThreadTerminate);

    // 线程退出管理。
    // 4.1.什么都不做。主线程退出时会直接回收子线程t

    // 4.2.join需要主线程等待子线程t退出后才结束。
    //t.join();

    // 4.3.detach使得线程t被交给C++运行时库。主线程退出仍可独自运行t。
    t.detach();


    // 主线程延时退出。可测试join和detach效果
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //延时1s
    std::cout << "main process terminated. \n";

    return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
