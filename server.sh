docker build --platform linux/amd64 -t nibbler .

docker run --platform linux/amd64 -p 8080:8080 nibbler
