package models

import (
	"errors"
	"fmt"
	"gorm.io/gorm"
	"smart-light.com/db"
	"time"
)

type Light struct {
	ID       uint      `gorm:"primaryKey;autoIncrement"`
	IsOpen   int       `gorm:"not null"` // 枚举值 0 和 1
	ExecTime time.Time `gorm:"not null"`
}

type DailyDuration struct {
	Date     string `json:"date"`
	Duration int64  `json:"duration"` // 时长（秒）
}

func CreateTurnOnRecordTable() {
	err := db.DB.AutoMigrate(&Light{})
	if err != nil {
		panic("failed to migrate database")
	}
}

func InsertRecord(data Light) error {
	var existingRecord Light
	err := db.DB.Where("is_open = ? AND exec_time = ?", data.IsOpen, data.ExecTime).First(&existingRecord).Error
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			if err := db.DB.Create(&data).Error; err != nil {
				fmt.Println("Failed to insert record:", err)
				return err
			} else {
				fmt.Println("Record inserted:", data)
				return nil
			}
		} else {
			fmt.Println("Error querying record:", err)
			return err
		}
	} else {
		fmt.Println("Record already exists, skipping insertion.")
		return err
	}
}

func GetCurrentStatus() (Light, error) {
	var record Light
	err := db.DB.Last(&record).Error
	if err != nil {
		if errors.Is(err, gorm.ErrRecordNotFound) {
			fmt.Println("No records found.")
			return record, nil
		} else {
			fmt.Println("Error querying record:", err)
			return record, err
		}
	}
	return record, err
}

func QueryLightDuration(page int) ([]DailyDuration, error) {
	var results []DailyDuration

	// Pagination variables
	offset := (page - 1) * 7 // Skip (page-1)*7 days for pagination

	// Custom SQL query
	err := db.DB.Raw(`
		WITH RECURSIVE dates AS (
		  SELECT date((SELECT MAX(date(exec_time)) FROM lights)) AS date
		  UNION ALL
		  SELECT date(date, '-1 day')
		  FROM dates
		  WHERE date > date((SELECT MAX(date(exec_time)) FROM lights), '-6 days')
		),
		paired AS (
		  SELECT
			exec_time AS start_time,
			LEAD(exec_time) OVER (ORDER BY exec_time) AS end_time,
			date(exec_time) AS date,
			is_open
		  FROM lights
		),
		durations AS (
		  SELECT
			date,
			CAST(
			  SUM(
				CASE
				  WHEN is_open = 1 AND end_time IS NOT NULL THEN
					(julianday(end_time) - julianday(start_time)) * 86400
				  ELSE 0
				END
			  ) AS INTEGER
			) AS duration
		  FROM paired
		  GROUP BY date
		)
		SELECT
		  d.date,
		  COALESCE(duration, 0) AS duration
		FROM dates d
		LEFT JOIN durations dur ON d.date = dur.date
		ORDER BY d.date DESC
		LIMIT 7 OFFSET ?;
	`, offset).Scan(&results).Error

	return results, err
}
