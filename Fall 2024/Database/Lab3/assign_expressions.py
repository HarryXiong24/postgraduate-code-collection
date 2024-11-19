import sqlite3
from relational_algebra import *


class Expressions:
    # You can manually define expressions here or load from the uploaded file
    # Example Query: Find Databases conference and their rankings
    sample_query = Projection(
        ThetaJoin(
            Selection(Relation("field_conference"), Equals("field", "Databases")),
            Relation("conference_ranking"),
            Equals("field_conference.conference", "conference_ranking.conf_abbr"),
        ),
        ["conference_ranking.conf_abbr", "conference_ranking.rank"],
    )

    # Uncomment and define other expressions

    # Question 1
    # List “University of California, Berkeley” faculty who have written conference papers between 2010 - 2024 in field “Databases”(include 2010 and 2024). The answer should be in the following format.
    expression1 = Projection(
        ThetaJoin(
            Selection(
                Relation("author"),
                Equals("author.affiliation", "University of California, Berkeley"),
            ),
            ThetaJoin(
                Selection(
                    Relation("pub_info"),
                    And(
                        GreaterEquals("pub_info.year", 2010),
                        LessEquals("pub_info.year", 2024),
                    ),
                ),
                Selection(
                    Relation("field_conference"),
                    Equals("field_conference.field", "Databases"),
                ),
                Equals("pub_info.conference", "field_conference.conference"),
            ),
            Equals("author.name", "pub_info.name"),
        ),
        ["author.name", "pub_info.conference", "pub_info.year", "pub_info.count"],
    )

    # Question 2
    # List all conferences in “Computer Science” major where no “University of California, Irvine” faculty has ever published.
    expression2 = Projection(
        Difference(
            Projection(
                Selection(
                    Relation("field_conference"),
                    Equals("field_conference.major", "Computer Science")
                ),
                ["field_conference.conference"]
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
    # Step 1: 获取所有 "Databases" 会议
    databases_conferences = Projection(
        Selection(
            Relation("field_conference"),
            Equals("field", "Databases")
        ),
        ["field_conference.conference"]
    )

    # 重命名 databases_conferences 中的 'conference' 属性为 'db_conference'
    databases_conferences = Rename(
        databases_conferences,
        mapping={"conference": "db_conference"}
    )

    # Step 2: 将 pub_info 与重命名后的 databases_conferences 进行连接
    pub_databases = ThetaJoin(
        Relation("pub_info"),
        databases_conferences,
        Equals("pub_info.conference", "db_conference")
    )

    # Step 3: 为自连接准备关系，重命名 pub_databases 的属性
    pub_databases_renamed = Rename(
        pub_databases,
        mapping={
            "name": "name_1",
            "conference": "conference_1",
            "year": "year_1",
            "count": "count_1",
        }
    )

    # Step 4: 找出发表论文数量更多的作者组合
    author_pairs = ThetaJoin(
        pub_databases,
        pub_databases_renamed,
        And(
            And(
              Equals("conference", "conference_1"),
              Equals("year", "year_1"),
            ),
            LessThan("count", "count_1")
        )
    )

    # Step 5: 找出没有比他们发表更多论文的作者
    max_authors = Projection(
        pub_databases,
        ["name", "conference", "year", "count"]
    ) - Projection(
        author_pairs,
        ["name", "conference", "year", "count"]
    )

    # Step 6: 获取作者的所属学校信息
    result_with_affiliation = NaturalJoin(
        max_authors,
        Relation("author")
    )

    # Step 7: 投影所需的列
    expression3 = Projection(
        result_with_affiliation,
        ["conference", "year", "name", "affiliation"]
    )

    # Question 4
    # Identify names of universities, where for all “Databases” conferences, there is a publication by an author affiliated with the university between 2020 - 2024(include 2020 and 2024). For example, if an author affiliated with “University of California, Irvine” published in SIGMOD in 2021, another author published in VLDB in 2022, another author published in PODS in 2023, and yet another author published in ICDE in 2022, then University of California, Irvine should be included in the result assuming ICDE, SIGMOD, PODS and VLDB are the only “Databases” conferences in the database.
    
    # Step 1: 获取所有 "Databases" 领域的会议名称
    databases_conferences = Projection(
        Selection(
            Relation("field_conference"),
            Equals("field_conference.field", "Databases")
        ),
        ["field_conference.conference"]
    )

    # Step 2: 筛选 2020 - 2024 年间的发表记录，并与作者的大学信息关联
    author_publications = Projection(
        Selection(
            NaturalJoin(
                Relation("pub_info"),
                Relation("author")
            ),
            And(
                GreaterEquals("pub_info.year", 2020),
                LessEquals("pub_info.year", 2024)
            )
        ),
        ["author.affiliation", "pub_info.conference"]
    )

    # Step 3: 选择那些会议在 "Databases" 领域中，并与作者发表记录匹配
    universities_with_publications = Projection(ThetaJoin(
        author_publications,
        databases_conferences,
        Equals("pub_info.conference", "field_conference.conference")
    ),
    ["author.affiliation","field_conference.conference"]
    )

    # Step 4: 找出符合条件的大学（需要使用 set division）
    result = Projection(
      (universities_with_publications
       / Projection(databases_conferences, ["field_conference.conference"])),
      ["author.affiliation"]
    )
    
    expression4 =  result

    # Question 5
    # Find all universities ranked(US News) better than “University of California, Irvine” such that the authors those schools have never published in conferences marked A*(ICore) in the field “Databases”.

   # 步骤 1：获取 UCI 的排名
    uci_ranking_col = Projection(
        Selection(
            Relation("usnews_university_rankings"),
            Equals("university_name", "University of California, Irvine")
        ),
        ["rank"]
    )
    uci_ranking_col = Rename(uci_ranking_col, {"rank": "uci_rank"})

    # 步骤 2：获取排名高于 UCI 的大学
    higher_ranked_universities = Projection(
        ThetaJoin(
            Relation("usnews_university_rankings"),
            uci_ranking_col,
            LessThan("usnews_university_rankings.rank", "uci_rank")
        ),
        ["usnews_university_rankings.university_name"]
    )

    # 步骤 3：获取“数据库”领域的 A* 级会议
    databases_conferences = Selection(
        Relation("field_conference"),
        Equals("field", "Databases")
    )

    a_star_conferences = Selection(
        Relation("conference_ranking"),
        Equals("rank", "A*")
    )

    databases_a_star_conferences = Projection(
        ThetaJoin(
            databases_conferences,
            a_star_conferences,
            Equals("field_conference.conference", "conference_ranking.conf_abbr")
        ),
        ["conference_ranking.conf_abbr"]
    )

    # 步骤 4：找到在这些会议上发表过论文的发表信息
    publications_in_a_star_databases = ThetaJoin(
        Relation("pub_info"),
        databases_a_star_conferences,
        Equals("pub_info.conference", "conference_ranking.conf_abbr")
    )

    # 步骤 5：找到这些发表信息的作者
    authors_of_a_star_databases = ThetaJoin(
        Relation("author"),
        publications_in_a_star_databases,
        Equals("author.name", "pub_info.name")
    )

    # 步骤 6：获取这些作者所属的大学
    universities_with_a_star_authors = Projection(
        authors_of_a_star_databases,
        ["author.affiliation"]
    )
    universities_with_a_star_authors = Rename(
        universities_with_a_star_authors,
        {"affiliation": "university_name"}
    )

    # 步骤 7：将这些大学与排名高于 UCI 的大学取交集
    universities_to_exclude = universities_with_a_star_authors & higher_ranked_universities

    # 步骤 8：从排名高于 UCI 的大学中减去需要排除的大学
    expression5 = higher_ranked_universities - universities_to_exclude
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
    # sql_con = sqlite3.connect('sample220P.db')  # Ensure the uploaded database is loaded here
    # result = sample_query.evaluate(sql_con=sql_con)
    # print(result.rows)
