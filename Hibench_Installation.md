# Tools installation
## install scala
go to `~/Program` directory
```bash
wget https://scala-lang.org/files/archive/scala-2.10.4.tgz
tar -xzvf scala-2.10.4.tgz
mv scala-2.10.4.tgz scala

# configure .bashrc
export SCALA_HOME=$WORK_SPACE/scala-2.10.4
export PATH=$PATH:$SCALA_HOME/bin

# Test
source .bashrc
scala -version
```

## Install Spark
```bash
cd /usr/local
sudo wget http://mirrors.ocf.berkeley.edu/apache/spark/spark-2.2.0/spark-2.2.0-bin-hadoop2.7.tgz
tar -zxvf spark-2.2.0-bin-hadoop2.7.tgz
```

# Reference
[Spark On YARN 集群安装部署](http://wuchong.me/blog/2015/04/04/spark-on-yarn-cluster-deploy/)
