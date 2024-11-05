import sqlite3
from relational_algebra import *
class Expressions:
    # You can manually define expressions here or load from the uploaded file
    # Example Query: Find Databases conference and their rankings
    sample_query = Projection(
            ThetaJoin(
                Selection(
                    Relation("field_conference"),
                    Equals("field", "Databases")
                ),
                Relation("conference_ranking"),
                Equals("field_conference.conference", "conference_ranking.conf_abbr")
            ),
            ["conference_ranking.conf_abbr", "conference_ranking.rank"]
        )

    # Uncomment and define other expressions
    
    # Question 1
    # List “University of California, Berkeley” faculty who have written conference papers between 2010 - 2024 in field “Databases”(include 2010 and 2024). The answer should be in the following format.
    expression1 = Projection(
        ThetaJoin(
            ThetaJoin(
                Selection(
                    Relation("author"),
                    Equals("author.affiliation", "University of California, Berkeley")
                ),
                ThetaJoin(
                    Selection(
                        Relation("pub_info"),
                        And(
                            GreaterEquals("pub_info.year", 2010),
                            LessEquals("pub_info.year", 2024)
                        )
                    ),
                    Selection(
                        Relation("field_conference"),
                        Equals("field_conference.field", "Databases")
                    ),
                    Equals("pub_info.conference", "field_conference.conference")
                ),
                Equals("author.name", "pub_info.name")
            ),
            Relation("field_conference"),
            Equals("pub_info.conference", "field_conference.conference")
        ),
        ["author.name", "pub_info.conference", "pub_info.year", "pub_info.count"]
    )
    
    # Question 2
    # List all conferences in “Computer Science” major where no “University of California, Irvine” faculty has ever published.
    expression2 = Projection(
        Difference(
            Selection(
                Relation("field_conference"),
                Equals("field_conference.major", "Computer Science")
            ),
            Projection(
                ThetaJoin(
                    ThetaJoin(
                        Selection(
                            Relation("author"),
                            Equals("author.affiliation", "University of California, Irvine")
                        ),
                        Relation("pub_info"),
                        Equals("author.name", "pub_info.name")
                    ),
                    Relation("field_conference"),
                    Equals("pub_info.conference", "field_conference.conference")
                ),
                ["field_conference.conference"]
            )
        ),
        ["field_conference.conference"]
    )
    
    # Question 3
    # There is no answer for expression3, becasue basic relational algebra uses the set semantic.
    # And It does not support aggregation operations like COUNT, SUM, AVG, etc.
    # In the question, we need to find the number of papers by authors in each conference each year, and then filter out the authors with the highest number of papers in each conference each year.
    # This is a typical aggregation operation, so it is not supported by basic relational algebra.
    # expression3 = ...
        
    # Question 4
    # Identify names of universities, where for all “Databases” conferences, there is a publication by an author affiliated with the university between 2020 - 2024(include 2020 and 2024). For example, if an author affiliated with “University of California, Irvine” published in SIGMOD in 2021, another author published in VLDB in 2022, another author published in PODS in 2023, and yet another author published in ICDE in 2022, then University of California, Irvine should be included in the result assuming ICDE, SIGMOD, PODS and VLDB are the only “Databases” conferences in the database.
    databases_conferences = Projection(
        Selection(
            Relation("field_conference"),
            Equals("field_conference.field", "Databases")
        ),
        ["field_conference.conference"]
    )
    
    university_publications = Projection(
        ThetaJoin(
            ThetaJoin(
                Selection(
                    Relation("author"),
                    And(
                        GreaterEquals("pub_info.year", 2020),
                        LessEquals("pub_info.year", 2024)
                    )
                ),
                Relation("pub_info"),
                Equals("author.name", "pub_info.name")
            ),
            databases_conferences,
            Equals("pub_info.conference", "field_conference.conference")
        ),
        ["author.affiliation", "pub_info.conference"]
    )
    
    missing_publications = Difference(
        databases_conferences,
        university_publications
    )

    expression4 = Projection(
        Difference(
            Projection(Relation("author"), ["author.affiliation"]),
            Projection(missing_publications, ["author.affiliation"])
        ),
        ["author.affiliation"]
    )
    
    # Question 5
    # Find all universities ranked(US News) better than “University of California, Irvine” such that the authors those schools have never published in conferences marked A*(ICore) in the field “Databases”.
    
    better_ranked_universities = Projection(
        Selection(
            Relation("usnews_university_rankings"),
            GreaterThan("usnews_university_rankings.rank", 
                Projection(
                    Selection(
                        Relation("usnews_university_rankings"),
                        Equals("usnews_university_rankings.university_name", "University of California, Irvine")
                    ),
                    ["usnews_university_rankings.rank"]
                )
            )
        ),
        ["university_ranking.university_name"]
    )
    
    databases_a_star_conferences = Projection(
        ThetaJoin(
            Selection(
                Relation("field_conference"),
                Equals("field", "Databases")
            ),
            Selection(
                Relation("conference_ranking"),
                Equals("rank", "A*")
            ),
            Equals("field_conference.conference", "conference_ranking.conf_abbr")
        ),
        ["conference_ranking.conf_abbr"]
    )
    
    universities_with_a_star_publications = Projection(
        ThetaJoin(
            Relation("author"),
            Relation("pub_info"),
            And(
                Equals("author.name", "pub_info.name"),
                In("pub_info.conference", databases_a_star_conferences)
            )
        ),
        ["author.affiliation"]
    )
    
    expression5 = Difference(
        Projection(better_ranked_universities, ["university_name"]),
        universities_with_a_star_publications
    )
    
    # Bonus Question
    
    # 1. General University Ranking Comparison
    # Method: Using the usnews_university_rankings table, which includes data like ranking, acceptance rate, and tuition, we can sort and compare universities based on overall reputation.
    # Graduate Applicant: General rankings give students a sense of a school’s overall reputation. Combined with acceptance rates and tuition costs, students can better estimate their chances of getting in and the financial commitment involved.
    # New Faculty Job Seeker: Schools that rank higher often have more resources and funding, which can be signs of better job stability, support for research, and room for growth.

    # 2. University Comparison by given major
    # Method: Using conference rankings from field_conference and conference_ranking tables, we can find out which universities lead in a specific field (like "Databases"). Tracking how often they publish in top conferences (like A* level) shows how active they are in that field.
    # Graduate Applicant: For students wanting to focus on a particular area, schools with more publications in that field’s top conferences are often better choices. These schools are likely to have more research resources and faculty expertise in the area they’re interested in.
    # New Faculty Job Seeker: Job candidates can use this information to see if a university’s research focus matches their own. This helps them target schools that fit with their research interests and goals.

    # Example query execution
    sql_con = sqlite3.connect('sample220P.db')  # Ensure the uploaded database is loaded here
    result = sample_query.evaluate(sql_con=sql_con)
    print(result.rows)
