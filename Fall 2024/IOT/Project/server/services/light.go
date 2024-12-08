package services

import (
	"fmt"
	"github.com/gin-gonic/gin"
	"net/http"
	"smart-light.com/models"
	"strconv"
	"time"
)

func HandleStatus(c *gin.Context) {

	isOpen := c.Query("isOpen")

	if isOpen == "" {
		c.JSON(http.StatusInternalServerError, gin.H{"status": 1, "error": "could not get parameter"})
		return
	}

	isOpenNumber, err := strconv.Atoi(isOpen)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"status": 1,
			"error":  "'age' must be a valid integer",
		})
		return
	}

	record := models.Light{
		IsOpen:   isOpenNumber,
		ExecTime: time.Now(),
	}

	err = models.InsertRecord(record)
	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"status": 1, "error": "could not insert record"})
		fmt.Println(err)
		return
	}

	c.JSON(http.StatusOK, gin.H{"status": 0, "isOpen": record.IsOpen, "execTime": record.ExecTime})
}

func ControlLight(c *gin.Context) {
	record, err := models.GetCurrentStatus()

	if err != nil {
		c.JSON(http.StatusInternalServerError, gin.H{"status": 1, "error": "could not get current status"})
		fmt.Println(err)
		return
	}

	c.JSON(http.StatusOK, gin.H{"status": 0, "isOpen": record.IsOpen, "execTime": record.ExecTime})
}

func GetTimeUsage(c *gin.Context) {

	page := c.Query("page")

	if page == "" {
		c.JSON(http.StatusInternalServerError, gin.H{"status": 1, "error": "could not get parameter"})
		return
	}

	pageNumber, err := strconv.Atoi(page)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"status": 1,
			"error":  "'age' must be a valid integer",
		})
		return
	}

	results, err := models.QueryLightDuration(pageNumber)
	if err != nil {
		fmt.Println("Error:", err)
		c.JSON(http.StatusOK, gin.H{"status": 0, "data": []models.DailyDuration{}})
		return
	}

	c.JSON(http.StatusOK, gin.H{"status": 0, "data": results})
}
