using System.Text.Json;
using System.Text.Json.Serialization;

namespace Leella.Prelude.Core;

public static class ProjectSerializer
{
    private static readonly JsonSerializerOptions Options = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        Converters = { new JsonStringEnumConverter(JsonNamingPolicy.CamelCase) }
    };

    public static async Task SaveAsync(PreludeProject project, string path, CancellationToken cancellationToken = default)
    {
        Directory.CreateDirectory(Path.GetDirectoryName(Path.GetFullPath(path))!);
        await using var stream = File.Create(path);
        await JsonSerializer.SerializeAsync(stream, project, Options, cancellationToken);
    }

    public static async Task<PreludeProject> LoadAsync(string path, CancellationToken cancellationToken = default)
    {
        await using var stream = File.OpenRead(path);
        return await JsonSerializer.DeserializeAsync<PreludeProject>(stream, Options, cancellationToken)
            ?? throw new InvalidDataException("르엘라 프렐류드 프로젝트 파일을 읽을 수 없습니다.");
    }
}
