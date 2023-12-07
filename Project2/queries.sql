SELECT Fname, Lname 
	FROM Players 
	WHERE Debut IS NOT NULL AND FinalGame IS NOT NULL
	ORDER BY FinalGame - Debut DESC LIMIT 1;

SELECT AVG(Height) 
	FROM Players 
	WHERE Debut >='01-01-1950';

SELECT Fname, Lname, G 
	FROM Players NATURAL JOIN Pitching 
	ORDER BY G DESC LIMIT 1;

SELECT Fname, Lname, year, Stint, TotalBase 
	FROM Players NATURAL JOIN 
		(SELECT PlayerID, year, stint, (SUM(H) + SUM(B2) + 2*SUM(B3) + 3*SUM(HR)) AS TotalBase
 			FROM BATTING 
			GROUP BY PlayerID, year, stint) AS Base;

SELECT Fname, Lname, Pi.year, Pi.stint, RBI 
	FROM Batting B, Players P, Pitching Pi 
	WHERE P.PlayerID = B.PlayerID 
		AND P.PlayerID = Pi.PlayerID 
		AND Pi.year = B.year 
		AND B.stint = Pi.stint 		
		AND RBI IS NOT NULL 
	ORDER BY RBI DESC LIMIT 5;

SELECT SB, AVG(weight) 
	FROM Players NATURAL JOIN Batting
	WHERE SB IS NOT NULL
	GROUP BY SB
	ORDER BY SB;

SELECT LgWin, SUM(R) 
	FROM Batting B, Teams T 
	WHERE B.TeamID = T.TeamID 
		AND B.year = T.year 
		AND Lgwin = 'Y'
	GROUP BY LgWin
UNION 
SELECT LgWin, SUM(R)
	FROM Batting B, Teams T
	WHERE B.TeamID = T.TeamID
		AND B.year = T.year
		AND LgWin = 'N'
	GROUP BY LgWin;

SELECT Name, year, pitcher
	FROM Teams NATURAL JOIN
		(SELECT TeamID, year, COUNT(DISTINCT PlayerID) AS pitcher FROM Pitching
			WHERE G >= 5
			GROUP BY TeamID, year HAVING COUNT(DISTINCT PlayerID) > 25) AS Player
	WHERE year >= 2000;
	
SELECT BirthMonth, COUNT(*) 
	FROM Players P, Teams T, Batting B 
	WHERE Name = 'Boston Red Sox' 
		AND T.year = B.year
		AND T.year = 2004
		AND T.TeamID = B.TeamID
		AND P.PlayerID = B.PlayerID
	GROUP BY BirthMonth;

SELECT  S1.year, Fname, Lname, Name, SO
	FROM (SELECT year, MAX(SO) AS greatest_SO FROM Pitching GROUP BY Year) AS S1 NATURAL JOIN 
		(SELECT Pi.year, Fname, Lname, Name, SO FROM Players NATURAL JOIN Pitching AS Pi, Teams T WHERE Pi.TeamID = 			T.TeamID AND Pi.year = T.year) AS S2 
	WHERE S2.SO = S1.greatest_SO ORDER BY year;
