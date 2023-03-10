using Microsoft.AspNetCore.Components;
using Microsoft.AspNetCore.Components.Web;
using Dapper;
using Syncfusion.Blazor;

DefaultTypeMap.MatchNamesWithUnderscores = true;

var builder = WebApplication.CreateBuilder(args);

Console.WriteLine(builder.Configuration.GetConnectionString("RFID"));

// Add services to the container.
builder.Services.AddRazorPages();
builder.Services.AddServerSideBlazor();
builder.Services.AddSingleton<IDbService,DbService>();
builder.Services.AddScoped<IGebruikerService,GebruikerService>();
builder.Services.AddScoped<IArtikelService, ArtikelService>();
//builder.Services.AddSyncfusionBlazor();

//builder.Services.AddScoped<IDbService, DbService>();
//builder.Services.AddScoped<IGebruikerService, GebruikerService>();


var app = builder.Build();



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
