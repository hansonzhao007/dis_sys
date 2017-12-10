cat intsum*.txt | python mapper.py | python reducer.py
hadoop fs -rm -R /user/output
hadoop \
jar /home/osboxes/Program/hadoop/share/hadoop/tools/lib/hadoop-streaming-2.7.4.jar \
-mapper mapper.py \
-reducer reducer.py \
-input /user/data/intsum*.txt \
-output /user/output \
-file mapper.py \
-file reducer.py

hadoop fs -cat /user/output/*
