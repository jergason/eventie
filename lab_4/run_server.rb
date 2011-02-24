for i in 1..10 do
  `python generator.py --server paintball.cs.byu.edu --port 3001 -d 30 -l #{i} > web-lighttpd-#{i}.txt`
end


for i in 1..10 do
  `python generator.py --server paintball.cs.byu.edu --port 3000 -d 30 -l #{i} > web-eventie-#{i}.txt`
end
