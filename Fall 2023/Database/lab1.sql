use sampleMDSDB;

-- 2
-- select Patients.name, Diagnosis.disease from Patients, Diagnosis, Outcomes
-- where Patients.name = Diagnosis.pname and Diagnosis.pname = Outcomes.pname 
-- and Outcomes.result = 'true';

-- 3
-- select Diagnosis.disease, Patients.age from Patients, Diagnosis
-- where Patients.name = Diagnosis.pname and Patients.age > 50
-- GROUP BY Diagnosis.disease, Patients.age;

-- 4
-- Step 1: Calculate the overall average age
with OverallAverageAge AS (
    SELECT AVG(Patients.age) AS avg_age
    FROM Patients
    JOIN Diagnosis ON Patients.name = Diagnosis.pname
),

-- Step 2: Calculate average age for each disease
DiseaseAverageAge AS (
    SELECT Diagnosis.disease, AVG(Patients.age) AS disease_avg_age
    FROM Patients
    JOIN Diagnosis ON Patients.name = Diagnosis.pname
    GROUP BY Diagnosis.disease
)

-- Step 3: Compare the average age of each disease with the overall average age
SELECT disease
FROM DiseaseAverageAge, OverallAverageAge
WHERE disease_avg_age > avg_age;