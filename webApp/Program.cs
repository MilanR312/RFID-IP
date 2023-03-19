using Microsoft.AspNetCore.Components;
using Microsoft.AspNetCore.Components.Web;
using Dapper;
using System.Net;
using System.Net.Sockets;

DefaultTypeMap.MatchNamesWithUnderscores = true;

var builder = WebApplication.CreateBuilder(args);

Console.WriteLine(builder.Configuration.GetConnectionString("RFID"));

// Add services to the container.
builder.Services.AddRazorPages();
builder.Services.AddServerSideBlazor();
builder.Services.AddSingleton<IDbService,DbService>();
builder.Services.AddScoped<IGebruikerService,GebruikerService>();
builder.Services.AddScoped<IArtikelService, ArtikelService>();
builder.Services.AddScoped<IDeviceService, DeviceService>();
builder.Services.AddSingleton<TcpListener>( _ => new(IPAddress.Any,8090));


var app = builder.Build();

var listener = app.Services.GetRequiredService<TcpListener>();
listener.Start(10);

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error");
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}

app.UseHttpsRedirection();

app.UseStaticFiles();

app.UseRouting();

app.MapBlazorHub();
app.MapFallbackToPage("/_Host");

app.Run();
