# Tools installation
## install scala
go to `~/Program` directory
```bash
wget https://scala-lang.org/files/archive/scala-2.10.4.tgz
tar -xzvf scala-2.10.4.tgz
mv scala-2.10.4.tgz scala
```

## Setting up environment for scala
add following line to `~/.bashrc` file.
```bash
export SCALA_HOME=$WORK_SPACE/scala
export PATH=$PATH:$SCALA_HOME/bin
```

## Verifying the scala installation
```bash
source .bashrc
scala -version
```

## Install Spark
```bash
cd /usr/local
sudo wget http://mirrors.ocf.berkeley.edu/apache/spark/spark-2.2.0/spark-2.2.0-bin-hadoop2.7.tgz
tar -zxvf spark-2.2.0-bin-hadoop2.7.tgz
mv spark-2.2.0-bin-hadoop2.7 spark
```
## Setting up environment for spark
add following line to `~/.bashrc` file.
```bash
export SPARK_HOME=$WORK_SPACE/spark
export PATH=$PATH:$SCALA_HOME/bin
```
then go to `spark/conf` folder
```
export SCALA_HOME=~/Program/scala
export JAVA_HOME=$(readlink -f /usr/bin/java | sed "s:bin/java::")
export HADOOP_HOME=~/Program/hadoop
export HADOOP_CONF_DIR=$HADOOP_HOME/etc/hadoop

# This is important, spark-shell will report `WARN util.NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable` if lack following cmd.
export LD_LIBRARY_PATH=~/Program/hadoop/lib/native

SPARK_LOCAL_DIRS=~/Program/spark

```
## Verifying the Spark Installation
Write the following command for opening Spark shell.
```bash
$spark-shell
```
If spark is installed successfully then you will find the following output.

```bash
SSpark context Web UI available at http://10.211.55.3:4040
Spark context available as 'sc' (master = local[*], app id = local-1512005367714).
Spark session available as 'spark'.
Welcome to
____              __
/ __/__  ___ _____/ /__
_\ \/ _ \/ _ `/ __/  '_/
/___/ .__/\_,_/_/ /_/\_\   version 2.2.0
/_/

Using Scala version 2.11.8 (OpenJDK 64-Bit Server VM, Java 1.8.0_151)
Type in expressions to have them evaluated.
Type :help for more information.
scala>
```

Then go to `spark/sbin/start-all.sh` to start Spark.
check `jps`
```bash
osboxes@hanson:~/Program/spark/sbin$ jps
24946 SecondaryNameNode
21462 DataNode
3431 Worker
21578 ResourceManager
21323 NameNode
21836 NodeManager
3231 Master
3503 Jps
```
You should see `Master` and `Worker` process.

You can also go to http://localhost:8080 to see spark management page.
![spark](http://ww4.sinaimg.cn/mw690/81b78497jw1eqwa7uqndoj20za0p8afa.jpg)

run following command to test:
```
run-example SparkPi 10 --master local[2]
```
The output should look like this:

# Reference

[Spark On YARN 集群安装部署](http://wuchong.me/blog/2015/04/04/spark-on-yarn-cluster-deploy/)
