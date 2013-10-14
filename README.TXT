【注意】
1.每层依次递归解析，同级同名节点扔到数组中，支持解析![CDATA标记
2.两种模式: 一种支持从文件读取，一种支持从终端读取(直接执行进入,Ctrl+D结束并打印结果)

【参数】
--help
    -f  加载xml文件
    -c  紧凑输出

【用法】
./xml2josn
cat test.xml|./xml2json
./xml2json -f test.xml
./xml2json < test.xml

【依赖】
XMLParser v2.41
JsonCpp

【问题】
最底层叶子节点不能重复
