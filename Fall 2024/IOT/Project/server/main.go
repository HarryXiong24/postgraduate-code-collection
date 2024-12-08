package main

import (
	"github.com/gin-gonic/gin"
	"smart-light.com/db"
	"smart-light.com/models"
	"smart-light.com/routers"
)

func main() {
	db.ConnectDB()
	models.CreateTurnOnRecordTable()

	server := gin.Default()

	routers.RegisterRoutes(server)

	err := server.Run(":8080")
	if err != nil {
		return
	}
}
