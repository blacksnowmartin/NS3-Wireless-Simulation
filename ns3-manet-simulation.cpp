CommandLine cmd;
cmd.Parse (argc, argv);
NodeContainer c;
c.Create (100);
MobilityHelper mobility;
mobility.SetPositionAllocator ("ns3::RandomDiscPositionAllocator",
"X", StringValue ("100.0"),
"Y", StringValue ("100.0"),
"Rho", StringValue ("ns3::UniformRandomVariable[Min=0|Max=30]"));
mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
"Mode", StringValue ("Time"),
"Time", StringValue ("2s"),
"Speed", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
"Bounds", StringValue ("0|200|0|200"));
mobility.InstallAll ();
Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
MakeCallback (&amp;amp;amp;amp;amp;amp;amp;CourseChange));
[/et_pb_text][/et_pb_column][/et_pb_row][/et_pb_section]
