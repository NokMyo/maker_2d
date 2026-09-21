namespace Leella.Prelude.Core;

public enum ProjectGameMode
{
    PlatformRpg,
    BeltScrollRpg
}

public sealed class PreludeProject
{
    public int FormatVersion { get; set; } = 1;
    public string Name { get; set; } = "새 프로젝트";
    public ProjectGameMode GameMode { get; set; } = ProjectGameMode.PlatformRpg;
    public string StartMapId { get; set; } = "map_001";
    public List<MapDocument> Maps { get; set; } = [];
}

public sealed class MapDocument
{
    public string Id { get; set; } = Guid.NewGuid().ToString("N");
    public string Name { get; set; } = "새 맵";
    public int Width { get; set; } = 1920;
    public int Height { get; set; } = 1080;
    public List<PlatformSegment> Platforms { get; set; } = [];
    public List<EntityPlacement> Entities { get; set; } = [];
}

public sealed class PlatformSegment
{
    public double X1 { get; set; }
    public double Y1 { get; set; }
    public double X2 { get; set; }
    public double Y2 { get; set; }
    public bool DropThrough { get; set; } = true;
}

public enum EntityKind
{
    PlayerStart,
    Npc,
    Monster,
    Portal
}

public sealed class EntityPlacement
{
    public string Id { get; set; } = Guid.NewGuid().ToString("N");
    public EntityKind Kind { get; set; }
    public string DefinitionId { get; set; } = "";
    public double X { get; set; }
    public double Y { get; set; }
}
