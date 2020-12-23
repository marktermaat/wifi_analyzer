FROM python:3.6-alpine
RUN pip install pyserial influxdb

COPY src/ /app
WORKDIR /app

CMD [ "python", "/app/client.py" ]