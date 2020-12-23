cd client
docker build -t marktermaat/wifi_analyzer_client -f Dockerfile .
docker tag marktermaat/wifi_analyzer_client marktermaat/wifi_analyzer_client:0.1
docker push marktermaat/wifi_analyzer_client:0.1
docker push marktermaat/wifi_analyzer_client:latest
cd ..