# Smart Light API Doc

## Handle Light State


GET http://localhost:8080/handle-status

### Parameters

#### isOpen

0 - close light

1 - open light

### response

```json
{
  "execTime": "2024-11-22T20:27:02.340568-08:00",
  "isOpen": 0,
  "status": 0
}
```

## Control Light

GET http://18.212.189.40:8080/control-light

### response

```json
{
  "execTime": "2024-11-22T20:27:02.340568-08:00",
  "isOpen": 0,
  "status": 0
}
```

# Get Time Usage

GET http://18.212.189.40:8080/time-usage

### Parameters

#### page

int

### response

```json
{
  "status": 0,
  "data": [
    {
      "date": "2024-11-22",
      "duration": 7200
    },
    {
      "date": "2024-11-21",
      "duration": 10800
    },
    {
      "date": "2024-11-20",
      "duration": 7200
    }
  ]
}
```