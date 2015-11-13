#!/bin/sh
for id in 85632 53339; do
  for page in {1..30}; do
    phantomjs submissions.js $id $page 2>&1 | python3 compare.py
  done
done
