//引入命名空间
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

//定义命名空间 {...}
namespace CSharp_FirstTest
{
  //定义类 {...}
  class Program
  {
    //定义main函数 {...}
    static void Main(string[] args)
    {
      int age = 20;
      int hp = 60;
      string name = "kyleljj";
      float rate = 10.0F;
      Double rateD = 10.0D;

      Console.WriteLine("Hello World");
      Console.Write("Hello　World2");
      Console.WriteLine("两个数{0}+{1}={2}", 1, 2, 1 + 2);

      Console.WriteLine(name);
      Console.WriteLine(rate);
      Console.WriteLine(rateD);

      Console.ReadKey();
    }
  }
}
